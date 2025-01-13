# rocnikovy projekt - Jan Gottweis

`baseline` program len vyskusa vyhodit kazdu hranu z grafu a ak nie je kriticka ani graf nie je

zatial berie vstupy vo velmi konkretnom tvare jeden sposob ako ich vyrobit je napriklad:

najprv `source .bashrc`

potom `parse_graph INPUT.g6 GRAPH.in`

potom spustit `run_include baseline.cpp CRITICAL_CHECKER.cpp < GRAPH.in`

v tomto pripade teda `run_include baseline.cpp checker/checker-sat.cpp < GRAPH.in`

da sa porovnat dva rozne pristupy (zatial su len 2):
`compare CRITICAL_CHECKER.cpp GRAPH.in PROGRAM1.cpp PROGRAM2.cpp`

konkretne napriklad `compare checker/checker-sat.cpp vstupy/critical.in baseline.cpp 5cycle.cpp`

ked sa medzi behmi chce checker zmenit napriklad na `checker/checker-3n.cpp` treba vymazat skompilovany `baseline`, lebo ho neskompiluje nanovo :(
(dufam ze len zatial :) )

berie aj viac grafove vstupy, teda pokial `INPUT.g6` obsahuje viac grafov malo by to byt fajn.
Nejake prikladove vstupy su tam uz aj rozparsovane malo by z nich byt jasne v akom formate su 

# upgrade 5cycle
`5cycle` detekuje 5cykly a pre ne spusti sat solver len raz ak je jedna hrana kriticka - zatial jedina optimalizacia -
pre 5 cykly totiz plati, ak je jedna hrana kriticka, su vsetky hrany kriticke

# todo
nejake upgrady na baseline, co je teda zmyslom projektu