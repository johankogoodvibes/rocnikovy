#!/bin/bash

source .bashrc
LOG_FILE=""
BATCH_SIZE=1
TOTAL_SAT_RUNS=0

usage() {
    echo "Usage:"
    echo "  $0 [-l LOGFILE] [-b BATCH_SIZE] INPUT.g6 FILTERED_OUTPUT.g6"
    echo "  $0 [-l LOGFILE] [-b BATCH_SIZE] INPUT.g6 FILTERED_OUTPUT.g6 START END"
    exit 1
}

while getopts ":l:b:" opt; do
    case "$opt" in
        l) LOG_FILE="$OPTARG" ;;
        b) BATCH_SIZE="$OPTARG" ;;
        \?) echo "Unknown option: -$OPTARG"; usage ;;
        :) echo "Option -$OPTARG needs an argument"; usage ;;
    esac
done
shift $((OPTIND - 1))

if [ $# -ne 2 ] && [ $# -ne 4 ]; then
    usage
fi

if ! [[ "$BATCH_SIZE" =~ ^[1-9][0-9]*$ ]]; then
    echo "Error: batch size must be a positive integer"
    exit 1
fi

INPUT_FILE="$1"
FILTERED_FILE="$2"

if [ ! -f "$INPUT_FILE" ]; then
    echo "Error: input file '$INPUT_FILE' does not exist."
    exit 1
fi

if [ $# -eq 4 ]; then
    START="$3"
    END="$4"

    if ! [[ "$START" =~ ^[0-9]+$ && "$END" =~ ^[0-9]+$ ]]; then
        echo "Error: START and END must be positive integers."
        exit 1
    fi

    if [ "$START" -le 0 ] || [ "$END" -le 0 ] || [ "$START" -gt "$END" ]; then
        echo "Error: invalid range."
        exit 1
    fi
else
    START=1
    END=$(wc -l < "$INPUT_FILE")
fi

: > "$FILTERED_FILE"

# TMP_G6="tmp/g6"
# TMP_IN="tmp/in"
# TMP_OUT="tmp/out"

TMP_G6=$(mktemp)
TMP_IN=$(mktemp)
TMP_OUT=$(mktemp)
TMP_ERR=$(mktemp)

cleanup() {
    rm -f "$TMP_G6" "$TMP_IN" "$TMP_OUT" "$TMP_ERR"
}
trap cleanup EXIT

INDEX=0


process_batch() {
    local batch_start="$1"
    local count="$2"


    if [ "$count" -eq 0 ]; then
        return
    fi

    if ! parse_graph "$TMP_G6" "$count" > "$TMP_IN" 2>/dev/null; then
        echo "Graphs $batch_start-$((batch_start + count - 1)): parse_graph failed"
        return
    fi

    : > "$TMP_ERR"

    if [ -n "$LOG_FILE" ]; then
        run_include kempecycle-samecolor.cpp checker/checker-kissat.cpp \
            < "$TMP_IN" > "$TMP_OUT" 2> >(tee -a "$LOG_FILE" > "$TMP_ERR")
        run_status=$?
    else
        run_include kempecycle-samecolor.cpp checker/checker-kissat.cpp \
            < "$TMP_IN" > "$TMP_OUT" 2> >(tee "$TMP_ERR" >&2)
        run_status=$?
    fi

    if [ "$run_status" -ne 0 ]; then
        echo "Graphs $batch_start-$((batch_start + count - 1)): run_include failed"
        return
    fi

    batch_sat_runs=$(grep -Eo 'sat solver runs:[[:space:]]*[0-9]+' "$TMP_ERR" | tail -n1 | grep -Eo '[0-9]+')

    if [ -n "$batch_sat_runs" ]; then
        TOTAL_SAT_RUNS=$((TOTAL_SAT_RUNS + batch_sat_runs))
    fi

    local i=0
    while IFS= read -r packed; do
        IFS= read -r result <&3 || break

        graph_num=$((batch_start + i))

        if printf '%s\n' "$result" | grep -Eqi "is[[:space:]]*critical"; then
            echo "Graph $graph_num: critical"
            printf '%s\n' "$packed" >> "$FILTERED_FILE"
        else
            echo "Graph $graph_num: not critical"
        fi

        i=$((i + 1))
    done < "$TMP_G6" 3< "$TMP_OUT"

    if [ "$i" -lt "$count" ]; then
        echo "Warning: only $i output lines for batch starting at graph $batch_start"
    fi
}


INDEX=0
BATCH_COUNT=0
BATCH_START=0

: > "$TMP_G6"

while IFS= read -r G6LINE; do
    INDEX=$((INDEX + 1))
    GRAPH_NUM=$((START + INDEX - 1))

    if [ "$BATCH_COUNT" -eq 0 ]; then
        BATCH_START="$GRAPH_NUM"
        : > "$TMP_G6"
    fi

    printf '%s\n' "$G6LINE" >> "$TMP_G6"
    BATCH_COUNT=$((BATCH_COUNT + 1))

    if [ "$BATCH_COUNT" -eq "$BATCH_SIZE" ]; then
        process_batch "$BATCH_START" "$BATCH_COUNT"
        BATCH_COUNT=0
    fi
done < <(sed -n "${START},${END}p" "$INPUT_FILE")

if [ "$BATCH_COUNT" -gt 0 ]; then
    process_batch "$BATCH_START" "$BATCH_COUNT"
fi
echo "total sat solver runs: $TOTAL_SAT_RUNS"