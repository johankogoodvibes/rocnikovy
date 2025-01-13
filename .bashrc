export CXXFLAGS="-O2 -g -std=c++17 -Wall -Wextra -Wconversion \
-Wfatal-errors -fsanitize=undefined,address"
CXX="g++"
alias c="$CXX $CXXFLAGS"

function cr(){
    exe=${1%.*}
    make $exe && echo compiled && time ./$exe
}

function needs_recompile() {
    exe=$1                   # First argument is the executable
    shift                    # Shift arguments so that $@ now contains only source files
    source_files=("$@")      # Remaining arguments are the source files
    # Check if the executable exists
    if [ ! -f "$exe" ]; then
        return 0  # Needs recompilation if executable is missing
    fi

    # Check if any source file is newer than the executable
    for file in "${source_files[@]}"; do
        if [ "$file" -nt "$exe" ]; then
            return 0  # Needs recompilation if a source file is newer
        fi
    done

    return 1  # No recompilation needed
}


function run_include(){
    exe=${1%.*}
    full_path=$2
    path=$(dirname "$full_path")
    source=$(basename "$full_path")
    inlcude=${source%.*}
    make -C $path $inlcude.o
    if needs_recompile "$exe" "$exe.cpp" "$path/$inlcude.o" ;then
        echo $CXX $CXXFLAGS $exe.cpp $path/$inlcude.o -o $exe
        echo compiling "$exe" && $CXX $CXXFLAGS $exe.cpp $path/$inlcude.o -o $exe && echo compiled && echo && ASAN_OPTIONS=detect_leaks=0 ./$exe
    else
        echo "up to date" && echo && ASAN_OPTIONS=detect_leaks=0 ./$exe
    fi
}

function compare(){
    checker=$1
    in=$2
    shift 2
    source=("$@")
    for file in "${source[@]}"; do
        echo $file $checker $in 
        run_include $file $checker < $in
    done
}


function parse_graph(){
    graph=$1
    where=$2
    count=${3:-100}
    if [ "parse.cpp" -nt "parse" ] || [ "showg.c" -nt "showg" ];then
        gcc -o showg showg.c && make parse && echo compiled && ./showg < $graph | ./parse $count > $where
    else
        echo up to date && ./showg < $graph | ./parse > $where
    fi
}

