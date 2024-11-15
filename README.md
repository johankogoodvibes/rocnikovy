# rocnikovy projekt - Jan Gottweis

`baseline` program len vyskusa vyhodit kazdu hranu z grafu a ak nie je kriticka ani graf nie je

zatial berie vstupy vo velmi konkretnom tvare jeden sposob ako ich vyrobit je napriklad:

najprv `source .bashrc`

potom `parse_graph INPUT.g6 GRAPH.in`

potom spustit `run_include baseline.cpp CRITICAL_CHECKER.cpp < GRAPH.in`

v tomto pripade teda `run_include baseline.cpp checker/checker-sat.cpp < GRAPH.in`

ked sa medzi behmi chce checker zmenit napriklad na `checker/checker-3n.cpp` treba vymazat skompilovany `baseline`, lebo ho neskompiluje nanovo :(

berie aj viac grafove vstupy, teda pokial `INPUT.g6` obsahuje viac grafov malo by to byt fajn

# todo
nejake upgrady na baseline, co je teda zmyslom projektu