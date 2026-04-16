export CXXFLAGS="-O2 -g -std=c++17 -Wall -Wextra -Wconversion \
-Wfatal-errors -fsanitize=undefined,address"
CXX="g++"
alias c="$CXX $CXXFLAGS"
BINDIR="programy"
OUTDIR="vystupy"
ERRDIR="errdir"
TESTDIR="vstupy"
TIMEDIR="timedir"

function cr(){
    # mozno trochu nefunkcne
    local exe=${1%.*}
    g++ -o $BINDIR/$exe $exe.cpp && echo compiled && time ./$BINDIR/$exe
}

function needs_recompile() {
    local exe=$1                   # First argument is the executable
    shift                    # Shift arguments so that $@ now contains only source files
    local source_files=("$@")      # Remaining arguments are the source files
    # Check if the executable exists
    if [ ! -f "$BINDIR/$exe" ]; then
        return 0  # Needs recompilation if executable is missing
    fi

    # Check if any source file is newer than the executable
    for file in "${source_files[@]}"; do
        if [ "$file" -nt "$BINDIR/$exe" ]; then
            return 0  # Needs recompilation if a source file is newer
        fi
    done

    return 1  # No recompilation needed
}

function run_include() {
    mkdir -p "$BINDIR"

    local exe=${1%.*}
    local full_path=$2
    local path
    path=$(dirname "$full_path")
    local source
    source=$(basename "$full_path")
    local include=${source%.*}
    local prog_args=("${@:3}")

    make -C "$path" "$include.o" 1>&2

    if needs_recompile "$exe" "$exe.cpp" "$path/$include.o"; then
        echo "$CXX" "$CXXFLAGS" "$exe.cpp" "$path/$include.o" -o "$BINDIR/$exe" 1>&2
        echo "compiling $exe" 1>&2
        $CXX $CXXFLAGS "$exe.cpp" "$path/$include.o" -o "$BINDIR/$exe" || return 1
        echo "compiled" 1>&2
        echo 1>&2
    else
        echo "up to date" 1>&2
    fi

    if [ -n "${RUN_TIME_FILE:-}" ]; then
        /usr/bin/time -f "%e" -o "$RUN_TIME_FILE" "./$BINDIR/$exe" "${prog_args[@]}"
    else
        "./$BINDIR/$exe" "${prog_args[@]}"
    fi
}

function run_all(){
    mkdir -p $OUTDIR
    local checker=$1
    local input=$(basename "$2")
    shift 2
    local source=("$@")

    for file in "${source[@]}"; do
        echo "running $file < $input" 
        mkdir -p "$OUTDIR/${file%.*}"
        run_include $file $checker < $TESTDIR/$input > $OUTDIR/${file%.*}/${input%.*}.out
    done

    local first_file="$OUTDIR/${source[0]%.*}/${input%.*}.out"
    local all_match=true  # Flag to track if everything matches

    for file in "${source[@]:1}"; do  # Skip first file
        if ! cmp --silent "$first_file" "$OUTDIR/${file%.*}/${input%.*}.out"; then
            echo "Outputs differ: $first_file vs $OUTDIR/${file%.*}/${input%.*}.out"
            all_match=false  # Mark as different
        fi
    done
    if $all_match; then 
        echo "All outputs match"
        local out_file="$OUTDIR/${input%.*}.out"
        if [ -f "$out_file" ] && ! cmp --silent "$first_file" "$out_file"; then
            echo "ERROR - Previous output was different!"
            cp "$first_file" "$OUTDIR/${input%.*}.out2"
        else
            echo "SAVING RESULT - consistent with previous result"
            cp "$first_file" "$OUTDIR/${input%.*}.out"
        fi
    else 
        all_good=false
    fi
}

function compare(){
    mkdir -p "$OUTDIR" "$ERRDIR" "$TIMEDIR"

    usage() {
        echo "Usage:"
        echo "  $0 [-d] [-a] CHECKER SOURCES..."
        exit 1
    }

    local DBG_FLAG=""
    local ALL_FLAG=""
    local OPTIND=1
    while getopts ":da" opt; do
        case "$opt" in
            d) DBG_FLAG="--dbg" ;;
            a) ALL_FLAG="--all" ;;
            \?) echo "Unknown option: -$OPTARG"; usage ;;
            :) echo "Option -$OPTARG needs an argument"; usage ;;
        esac
    done
    shift $((OPTIND - 1))

    if [ $# -lt 2 ]; then
        usage
    fi

    local checker="$1"
    shift
    local source=("$@")

    local tests=()
    for file in "$TESTDIR"/test-*; do
        tests+=("$file")
    done

    local results=()
    local time_results=()
    local test_names=()

    for file in "${tests[@]}"; do
        local test_name
        test_name=$(basename "$file")
        test_names+=("$test_name")
    done

    for file in "${source[@]}"; do
        results+=("$(basename "$file")")
        time_results+=("$(basename "$file")")
    done
    
    local all_good=true

    for input in "${test_names[@]}"; do
        for i in "${!source[@]}"; do
            local comp_file=${source[$i]}
            echo "running $comp_file $DBG_FLAG $ALL_FLAG < $input"

            mkdir -p "$OUTDIR/${comp_file%.*}"
            mkdir -p "$ERRDIR/${comp_file%.*}"
            mkdir -p "$TIMEDIR/${comp_file%.*}"

            local out_file="$OUTDIR/${comp_file%.*}/${input%.*}.out"
            local err_file="$ERRDIR/${comp_file%.*}/${input%.*}.err"
            local time_file="$TIMEDIR/${comp_file%.*}/${input%.*}.time"

            RUN_TIME_FILE="$time_file" \
                run_include "$comp_file" "$checker" $DBG_FLAG $ALL_FLAG \
                < "$TESTDIR/$input" \
                > "$out_file" \
                2> "$err_file"

            local runs_count
            runs_count=$(grep "sat solver runs:" "$err_file" | awk '{print $4}')
            [ -z "$runs_count" ] && runs_count="-"

            local exec_time
            exec_time=$(cat "$time_file" 2>/dev/null)
            [ -z "$exec_time" ] && exec_time="-"

            results[$i]+=" $runs_count"
            time_results[$i]+=" $exec_time"
        done

        local first_file="$OUTDIR/${source[0]%.*}/${input%.*}.out"
        local all_match=true

        for file in "${source[@]:1}"; do
            if ! cmp --silent "$first_file" "$OUTDIR/${file%.*}/${input%.*}.out"; then
                echo "Outputs differ: $first_file vs $OUTDIR/${file%.*}/${input%.*}.out"
                all_match=false
            fi
        done

        if $all_match; then 
            echo "All outputs match"
            local saved_out="$OUTDIR/${input%.*}.out"
            if [ -f "$saved_out" ] && ! cmp --silent "$first_file" "$saved_out"; then
                echo "ERROR - Previous output was different!"
                cp "$first_file" "$OUTDIR/${input%.*}.out2"
            else
                echo "SAVING RESULT - consistent with previous result"
                cp "$first_file" "$saved_out"
            fi
        else 
            all_good=false
        fi
    done

    if $all_good; then
        echo "All programs output the same"

        printf "" > table.txt
        printf "" > table_time.txt

        printf "\n| %-15.15s |" "prog/in"
        printf "\n| prog/in |" >> table.txt
        printf "\n| prog/in |" >> table_time.txt

        for input in "${test_names[@]}"; do
            printf " %-15.15s |" "$input"
            printf " $input |" >> table.txt
            printf " $input |" >> table_time.txt
        done

        printf "\n| --------------- |"
        printf "\n| - |" >> table.txt
        printf "\n| - |" >> table_time.txt

        for _ in "${test_names[@]}"; do
            printf " --------------- |"
            printf " - |" >> table.txt
            printf " - |" >> table_time.txt
        done
        printf "\n"
        printf "\n" >> table.txt
        printf "\n" >> table_time.txt

        for r in "${results[@]}"; do
            IFS=' ' read -r program counts <<< "$r"

            printf "| %-15.15s |" "$program"
            printf "| $program |" >> table.txt

            local counts_array=()
            read -ra counts_array <<< "$counts"

            for count in "${counts_array[@]}"; do
                printf " %15.15s |" "$count"
                printf " $count |" >> table.txt
            done
            printf "\n"
            printf "\n" >> table.txt
        done

        for r in "${time_results[@]}"; do
            IFS=' ' read -r program times <<< "$r"

            printf "| $program |" >> table_time.txt

            local times_array=()
            read -ra times_array <<< "$times"

            for t in "${times_array[@]}"; do
                printf " $t |" >> table_time.txt
            done
            printf "\n" >> table_time.txt
        done
    else 
        echo "some programs output incorrect"
    fi
}

function parse_graph(){
    mkdir -p $BINDIR
    local graph=$1
    local count=${2:--1}
    if [ "parse.cpp" -nt "$BINDIR/parse" ] || [ "showg.c" -nt "$BINDIR/showg" ];then
        gcc -o $BINDIR/showg showg.c && g++ parse.cpp -o $BINDIR/parse && echo compiled 1>&2 && ./$BINDIR/showg < $graph | ./$BINDIR/parse $count
    else
        echo up to date 1>&2 && ./$BINDIR/showg < $graph | ./$BINDIR/parse $count
    fi
}

