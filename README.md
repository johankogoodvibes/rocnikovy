# Rocnikovy projekt - Jan Gottweis

`baseline` program len vyskusa vyhodit kazdu hranu z grafu a ak nie je kriticka ani graf nie je

zatial berie vstupy vo velmi konkretnom tvare jeden sposob ako ich vyrobit je napriklad:

```bash
source .bashrc
parse_graph INPUT.g6 GRAPH.in
run_include baseline.cpp CRITICAL_CHECKER.cpp < GRAPH.in

# konkretne teda napriklad
run_include baseline.cpp checker/checker-sat.cpp < GRAPH.in
```

da sa otestovat rozne pristupy na vytvorenych vstupoch:
budu sa behat vsetky vstupy v adresari vstupy s prefixom `test-`
spustit sa to da nasledovne:

```bash
compare checker/CHECKER.cpp PROGRAM1.cpp PROGRAM2.cpp ...

#teda konkretne zbehnut vsetky pristupy na vsetkych vstupoch sa da napriklad takto:

compare checker/checker-sat.cpp baseline.cpp 5cycle-nochain.cpp 5cycle.cpp kempecycle-nochain.cpp kempecycle-5cycle-nochain.cpp
```

da sa spustit aj viacero pristupov na jednom vstupe pomocou: 
```bash
run_all CHECKER.cpp INPUT.in SOURCE1.cpp ...

# napriklad teda
run_all checker/checker-sat.cpp vstupy/critical.in baseline.cpp kempecycle-5cycle-nochain.cpp
```

*je nevyhnutne pouzit checker-sat ostatne uz nefunguju (ale fungovali :p )*

berie aj viac grafove vstupy, teda pokial `INPUT.g6` obsahuje viac grafov malo by to byt fajn.
Nejake prikladove vstupy su aj rozparsovane malo by z nich byt jasne v akom formate su 

## upgrade 5cycle
detekuje 5cykly a pre ne spusti sat solver len raz ak je jedna hrana kriticka - zatial jedina optimalizacia -
pre 5 cykly totiz plati, ak je jedna hrana kriticka, su vsetky hrany kriticke

### 5cycle-nochain
implementuje tuto myslienku a ked sat solver odhali kriticku hranu prehlasi aj vsetky na 5cykle s nou ze kriticke a uz ich neskusa

### 5cycle
to iste ako `5cycle-nochain` az na to ze ak je nejaka hrana prehlasena za kriticku bez ohladu na to ci sat solverom alebo *najdenim 5cyklu*, prehlasi aj vsetky hrany s nou v 5cykle za kriticke.

## upgrade kempecycle
*nevedel som najst lepsi nazov*

ked najdeme kriticku hranu a pozrieme sa na farbenie zvysku grafu ako na cirkulaciu v poli Z2xZ2 teda kriticka hrana ma hodnotu 0 a pre kazdy vrchol plati ze sucet jeho hran je 0, vieme si lahko vsimnut ze aj niektore dalsie hrany su kriticke takto:

pre koncove vrcholy nasej hrany nech su to a, b, vieme ze jedna hrana z nich ma hodnotu 0, ostatne 2 maju rovnaku hodnotu. ked teraz tvorime kempeho cestu z a, lahko dokazeme ze sa zacyklime naspat do a, rovnako aj pre b.

ak kempeho "cyklus" z a mal rovnake hodnoty hran ako ten z b a existuje hrana spajajuca vrchol s cyklu pre a a s cyklu pre b, mame cyklus pre ktory po aplikovani kempe switchu dostaneme validne ohodnotenie v nasom poli ale 0 bude na inej hrane teda dostavame ze aj ta je kriticka

### kempecycle-nochain
implementuje tuto myslienku s tym ze zatial prehlasi hrany za kriticke ale uz nepokracuje s algoritmom aj pre ne

### kempecycle-5cycle-nochain
aplikuje aj `kempecycle-nochain` aj `5cycle-nochain` optimalizacie

### kempecycle
ked sat solverom zisti ze hrana je kriticka a najde cyklus kde kempeswitchom najdeme inu cirkulaciu s inou nulovou hranou vykona tento kempeswitch a skusi rekurzivne riesit dalej s novou nulovou hranou

### kempecycle-samecolor

ak po najdeni cirkulacie pre nulovu hranu $a, b$ ma BUNV vrchol $a$ hrany farieb 0, 1, 1 a vrchol b hrany farieb 0, 2, 2, vieme aplikovat kempeswitch na cyklus z b v ktorom sa striedaju farby 1, 2 cim dostaneme cirkulaciu kde ma aj b farby 0, 1, 1. potom mame na vyber viac moznosti ked hladame predoslym algoritmom dalsie kriticke hrany

## performance

_všetky hodnoty sú v počtoch behu algoritmu na hľadanie farbenia_

| prog/in | test-0-petersen.in | test-1-critical.in | test-2-not_critical40.in | test-3-not_critical74.in | test-4-random38.in1 | test-5-random.in2 | test-6-velkecritical.in | test-7-jozkove_critical.in |
| - | - | - | - | - | - | - | - | - |
| baseline.cpp | 15 | 3831 | 415 | 6 | 1345 | 49 | 27000 | 2604 |
| 5cycle-nochain.cpp | 3 | 1555 | 415 | 4 | 1081 | 47 | 18030 | 2604 |
| 5cycle.cpp | 1 | 1378 | 415 | 4 | 1081 | 47 | 17375 | 2604 |
| kempecycle-nochain.cpp | 5 | 1364 | 192 | 5 | 1201 | 48 | 8493 | 855 |
| kempecycle-5cycle-nochain.cpp | 3 | 1017 | 192 | 3 | 1013 | 46 | 7506 | 855 |
| kempecycle.cpp | 1 | 123 | 54 | 3 | 739 | 46 | 879 | 82 |
| kempecycle-samecolor.cpp | 1 | 100 | 51 | 2 | 693 | 45 | 512 | 12 |


pre velke grafy - napriklad 3000 vrcholove - ptest-10 a 11 dlho trva zbehnut uz len ten sat solver a zvysok je vacsinou instantny

## todo
nejake upgrady na baseline, co je teda zmyslom projektu

- implementovat chaining do kempecycle aj 5cycle zaroven - treba vediet ako by farbenie vyzeralo pre kriticke hrany na 5 cykle
- kempecycle viem iterovat, ak vhodne pokombinujem kempeho cykly ktore mozu mat rozne farby tak viem najst nejaku novu kriticku hranu

skusit optimalizovat satsolver ale to neni asi podstatne