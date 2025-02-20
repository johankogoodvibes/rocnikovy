export CXXFLAGS="-O2 -g -std=c++17 -Wall -Wextra -Wconversion \
-Wfatal-errors -fsanitize=undefined,address"
CXX="g++"
alias c="$CXX $CXXFLAGS"
BINDIR="programy"
OUTDIR="vystupy"

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
        echo compiling "$exe" 1>&2 && $CXX $CXXFLAGS $exe.cpp $path/$inlcude.o -o $BINDIR/$exe && echo compiled 1>&2 && echo && ASAN_OPTIONS=detect_leaks=0 ./$BINDIR/$exe
    else
        echo "up to date" 1>&2 && ASAN_OPTIONS=detect_leaks=0 ./$BINDIR/$exe
    fi
}

function compare(){
    mkdir -p $OUTDIR
    local checker=$1
    local in=$2
    shift 2
    local source=("$@")
    for file in "${source[@]}"; do
        run_include $file $checker < $in > $OUTDIR/${file%.*}.out
    done

    local first_file="$OUTDIR/${source[0]%.*}.out"
    local all_match=true  # Flag to track if everything matches

    for file in "${source[@]:1}"; do  # Skip first file
        if ! cmp --silent "$first_file" "$OUTDIR/${file%.*}.out"; then
            echo "Outputs differ: $first_file vs $OUTDIR/${file%.*}.out"
            all_match=false  # Mark as different
        fi
    done

    # Print only if no mismatches were found
    $all_match && echo "All outputs match"

}


function parse_graph(){
    mkdir -p $BINDIR
    local graph=$1
    local where=$2
    local count=${3:-100}
    if [ "parse.cpp" -nt "$BINDIR/parse" ] || [ "showg.c" -nt "$BINDIR/showg" ];then
        gcc -o $BINDIR/showg showg.c && g++ parse.cpp -o $BINDIR/parse && echo compiled && ./$BINDIR/showg < $graph | ./$BINDIR/parse $count > $where
    else
        echo up to date && ./$BINDIR/showg < $graph | ./$BINDIR/parse > $where
    fi
}

