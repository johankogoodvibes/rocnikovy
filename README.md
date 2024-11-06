# rocnikovy projekt - Jan Gottweis

`baseline` program len vyskusa vyhodit kazdu hranu z grafu a ak nie je kriticka ani graf nie je

zatial berie vstupy vo velmi konkretnom tvare jeden sposob ako ich vyrobit je napriklad:

najprv `source .bashrc`

potom `parse_graph INPUT.g6 OUTPUT`

potom spustit `run_include baseline.cpp HLADANIE_FARBENIA.cpp < OUTPUT`

v tomto pripade teda `run_include baseline.cpp coloring/color-2n.cpp < OUTPUT`

berie aj viac grafove vstupy, teda pokial `INPUT.g6` obsahuje viac grafov malo by to byt fajn

# todo
nejake upgrady na baseline, co je teda zmyslom projektu

nejaky rozumny algoritmus na hladanie farbenia