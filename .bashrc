export CXXFLAGS="-O2 -g -std=c++17 -Wall -Wextra -Wconversion \
-Wfatal-errors -fsanitize=undefined,address"
CXX="g++"
alias c="$CXX $CXXFLAGS"
BINDIR="programy"
OUTDIR="vystupy"
ERRDIR="errdir"
TESTDIR="vstupy"

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

function run_include(){
    mkdir -p $BINDIR
    local exe=${1%.*}
    local full_path=$2
    local path=$(dirname "$full_path")
    local source=$(basename "$full_path")
    local inlcude=${source%.*}
    make -C $path $inlcude.o 1>&2
    if needs_recompile "$exe" "$exe.cpp" "$path/$inlcude.o" ;then
        echo $CXX $CXXFLAGS $exe.cpp $path/$inlcude.o -o $BINDIR/$exe 1>&2
        echo compiling "$exe" 1>&2 && $CXX $CXXFLAGS $exe.cpp $path/$inlcude.o -o $BINDIR/$exe && echo compiled 1>&2 && echo 1>&2 && ./$BINDIR/$exe
    else
        echo "up to date" 1>&2 && ./$BINDIR/$exe
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
    mkdir -p "$OUTDIR" "$ERRDIR"  # Create output and error directories
    local checker="$1"
    shift  # Shift only the checker argument
    local source=("$@")

    local tests=()
    for file in "$TESTDIR"/test-*; do  # Iterate through test files
        tests+=("$file")
    done

    local results=()
    local test_names=()

    for file in "${tests[@]}"; do
        test_name=$(basename "$file")
        test_names+=("$test_name")
    done


    for file in "${source[@]}"; do
        results+=("$(basename "$file")")
    done
    
    local all_good=true

    for input in ${test_names[@]}; do
        for i in "${!source[@]}"; do
            local comp_file=${source[$i]}
            echo "running $comp_file < $input" 
            mkdir -p "$OUTDIR/${comp_file%.*}"
            mkdir -p "$ERRDIR/${comp_file%.*}"
            run_include $comp_file $checker < $TESTDIR/$input > $OUTDIR/${comp_file%.*}/${input%.*}.out 2> "$ERRDIR/${comp_file%.*}/${input%.*}.err"
            local runs_count=$(grep "sat solver runs:" "$ERRDIR/${comp_file%.*}/${input%.*}.err" | awk '{print $4}')
            
            results[$i]+=" $runs_count"
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
    done
    if $all_good; then
        echo "All programs output the same"
        printf "" > table.txt
        printf "\n| %-15.15s |" "prog/in"  # Left-align, 10-character width for "prog/in"
        printf "\n| prog/in |" >> table.txt
        for input in "${test_names[@]}"; do
            printf " %-15.15s |" "$input" # Left-align, 15-character width for input names
            printf " $input |" >> table.txt
        done
        printf "\n| --------------- |"
        printf "\n| - |" >> table.txt
        for _ in "${test_names[@]}"; do
            printf " --------------- |"
            printf " - |" >> table.txt
        done
        printf "\n"
        printf "\n" >> table.txt
        for r in "${results[@]}"; do
            IFS=' ' read -r program counts <<< "$r"  # Split by spaces

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
    else 
        echo "some programs output incorrect"
    fi
}

function parse_graph(){
    mkdir -p $BINDIR
    local graph=$1
    local where=$2
    local count=${3:-100}
    if [ "parse.cpp" -nt "$BINDIR/parse" ] || [ "showg.c" -nt "$BINDIR/showg" ];then
        gcc -o $BINDIR/showg showg.c && g++ parse.cpp -o $BINDIR/parse && echo compiled && ./$BINDIR/showg < $graph | ./$BINDIR/parse $count > $where
    else
        echo up to date && ./$BINDIR/showg < $graph | ./$BINDIR/parse $count > $where
    fi
}

