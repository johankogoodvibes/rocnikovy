export CXXFLAGS="-O2 -g -std=c++17 -Wall -Wextra -Wconversion \
-Wfatal-errors -fsanitize=undefined,address template.cpp"
CXX="g++"
alias c="$CXX $CXXFLAGS"
function cr(){
    exe=${1%.*}
    make $exe && echo compiled && time ./$exe
}
function run_include(){
    exe=${1%.*}
    coloring=$2
    if [ "$exe.cpp" -nt "$exe" ] || [ "$coloring" -nt "$exe" ];then
        $CXX $CXXFLAGS $exe.cpp $coloring -o $exe && echo compiled && time ./$exe
    else
        echo "up to date" && time ./$exe
    fi
}
function test(){
    input=${1}
    coloring=$2
    baseline && make upgrade1
    echo compiled
    echo baseline && time ./baseline < $input
    echo upgrade1 && time ./upgrade1 < $input
}

function parse_graph(){
    graph=$1
    where=$2
    gcc -o showg showg.c && make parse && echo compiled && ./showg < $graph | ./parse > $where
}

