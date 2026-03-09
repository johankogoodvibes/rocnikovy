# Rocnikovy projekt - Jan Gottweis

## Ako to spustit

__usecase:__ mam .g6 subor a chcem snarky ktore su kriticke:

```bash
chmod +x filter_critical.sh # aby sa to dalo spustit

./filter_critical.sh vstup.g6 filtered.g6

./filter_critical.sh -l log vstup.g6 filtered.g6 # skoro vsetok vypis pojde do suboru log

./filter_critical.sh -l log -b 200 vstup.g6 filtered.g6 # teraz spracuje v batches po 200 snarkov

./filter_critical.sh -l log -b 200 vstup.g6 filtered.g6 1000 2000 # teraz spracuje zo vstupneho suboru len snarky 1000-2000
```

Da sa pomenit nejake veci:
1. ktory satsolver sa pouziva zatial je to kissat/zchaff - mozu produkovat rozne riesenia staci zmenit `checker/checker-kissat.cpp` za `checker/checker-sat.cpp`
2. nejake rozne pristupy co som kodil ale ten co je default je myslim najrychlejsi, `kempecycle-samecolor.cpp` zmenit za niektory iny napr.: `baseline.cpp`

## Nasleduju vsetky kecy

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

*je nevyhnutne pouzit checker-sat/checker-kissat ostatne uz nefunguju (ale fungovali :p )*

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

# Bakalarka - stale Jan Gottwies

Tuto zacina moja bakalarka, odvija sa to od programov z rocnikacu, ale skusime nejak lepsie analyzovat, co sa presne deje a preco sa to deje.

## Co ak graf nie je kriticky?

Zatial sme proste povedali ze kriticky nie je. To ale casto nepovie vela o tom, aky je nas algoritmus dobry, lebo vela krat tipneme hned na zaciatku hranu ktora nie je kriticka.
Preto sme skusili merat to ako dobry mame algoritmus pri nekritickych grafoch trochu inak: skusime pre kazdu hranu odhalit, ci sa ju da odstranit, alebo nie.
Zaujima nas kolko sat solverov potrebujeme kym odhalime prvu nekriticku hranu a kym to odhalime o vsetkych hranach.

### Novy performance - len vstupy s nekritickymi hranami

Ukazeme pocty potrebne pre verdikt o kazdej hrane - tie kde staci najst nekriticku hranu su v tabulke vyssie.

| prog/in | test-00-petersen.in | test-01-critical.in | test-02-not_critical40.in | test-03-not_critical74.in | test-04-random38.in1 | test-05-random.in2 | test-06-velkecritical.in | test-07-jozkove_critical.in |
| - | - | - | - | - | - | - | - | - |
| kempecycle-samecolor.cpp | 1 | 100 | 79 | 21 | 13589 | 3392 | 513 | 13 |
| kempecycle.cpp | 1 | 136 | 56 | 2 | 752 | 45 | 883 | 91 |
| kempecycle-nochain.cpp | 5 | 1367 | 196 | 5 | 1220 | 48 | 8323 | 819 |
| kempecycle-5cycle-nochain.cpp | 3 | 1046 | 196 | 4 | 1016 | 47 | 7250 | 819 |
| 5cycle.cpp | 1 | 1378 | 415 | 4 | 1081 | 47 | 17375 | 2604 |
| 5cycle-nochain.cpp | 3 | 1555 | 415 | 4 | 1081 | 47 | 18030 | 2604 |
| baseline.cpp | 15 | 3831 | 415 | 6 | 1345 | 49 | 27000 | 2604 |

*je to trochu odlisne ako predtym, lebo som pouzil iny sat solver*

## Skusime lepsi satsolver a vieme riesit vacsie vstupy

Pouzijeme kissat a vieme vpohode riesit aj velke vstupy, ale asi to nebudeme robit s tymi pomalymi programami, lebo to by trvalo velmi dlho predsalen

Nakodil som novy `checker-kissat.cpp` ktory pouziva tento novy checker - *todo tu budu aj nejake vysledky na vacsich vstupoch*

## Skontrolovat vsetky grafy

todo - chceli by sme vyskusat spustit niektore algoritmy na vsetkych znamych (malych) snarkoch, a povedat nejake ich vlastnosti. napr.: ktore z nich potrebuju viac ako jeden beh zafarbitelnosti a preco

## nedostatok kempecycle-samecolor
ked si prepinam hrany, aby som mal "viac moznosti" mam v skutocnosti len "asi viac moznosti". moze sa stat ze niektore moznosti stratim. vysledky ukazuju ze to je stale asi lepsie ako bez toho, ale nemusi to byt vzdy (protipriklad 148 graf z test-06)

## mal som chybu v kempecycle.cpp
ukladal som si do zoznamu vyriesenych hran usporiadane dvojice, teda obcas som musel hranu kontrolovat viac (2) krat

je viac moznosti ako to spravit. 
kempecycle-stary to robi tak ze si uklada usporiadane dvojice
kempecycle-swap to vymeni tak aby vzdy (a, b) platilo a < b
kempecycle-noswap to ulozi tak ze (a, b) a < b ale pocita tak ako mu to bolo zadane
(tie programy nie su ako samostatne subory, to by bolo zbytocne)
nove vysledky su tu:

| prog/in | test-00-petersen.in | test-01-critical.in | test-02-not_critical40.in | test-03-not_critical74.in | test-04-random38.in1 | test-05-random.in2 | test-06-velkecritical.in | test-07-jozkove_critical.in | test-08-mazak-dot50.in |
| - | - | - | - | - | - | - | - | - | - |
| kempecycle-samecolor.cpp | 1 | 100 | 50 | 2 | 695 | 45 | 513 | 13 | 105 |
| kempecycle-swap.cpp | 2 | 138 | 58 | 2 | 746 | 45 | 887 | 71 | 170 |
| kempecycle-noswap.cpp | 1 | 131 | 56 | 2 | 743 | 45 | 905 | 94 | 191 |
| kempecycle-stary.cpp | 1 | 136 | 56 | 2 | 752 | 45 | 883 | 91 | 180 |

*todo netusim aky je rozdiel logicky medzi swap a noswap, a preco maju rozdielne vysledky*
*uz tusim. tym ze hrana moze byt "naopak" mozem prechadzat cyklus opacnym smerom, co znamena ze niektore hrany najdem v inom poradi. a kedze to robim rekurzivne a kazdu hranu prehladavam len raz, tak mozem mat pre hranu rozne farbenia, co vedie k roznemu pokracovaniu*

ten stary moze mat mozno lepsie vysledky, lebo musim hranu prehladat akoby *z druhej strany* co ma nuti mat nejake ine farbenie najskor, co mozno znamena ze mam viac moznosti ako pokracovat. Ak som navyse nemusel hladat uplne nove farbenie, ale dopracoval som sa k tomu kempeswitchami, mozem mat lepsie riesenie, ako bez toho.

## je to nahodne
Ak zmenim sat solver casto najdem ine farbenie, a potom dostanem trosku iny vysledok. +-10% som to tak od oka odhadol.
Dolezite ale je, ze to nezavisi len od grafu, ale aj od toho ake farbenia dostavam

## dovolime viac krat skontrolovat tu istu hranu
preto mal stary algoritmus obcas lepsie riesenie, lebo mohol viac krat kontrolovat to iste. obcas ho to sice stalo nejake to hladanie farbenia navyse...
fixol som to ze mozem navstivit hranu viac krat ale staci ju navstivit len raz.
toto su vysledky pre `MAX_REPETITIONS=3`:

| prog/in | test-00-petersen.in | test-01-critical.in | test-02-not_critical40.in | test-03-not_critical74.in | test-04-random38.in1 | test-05-random.in2 | test-06-velkecritical.in | test-07-jozkove_critical.in | test-08-mazak-dot50.in |
| - | - | - | - | - | - | - | - | - | - |
| kempecycle-samecolor.cpp | 1 | 100 | 50 | 2 | 695 | 45 | 513 | 13 | 105 |
| kempecycle.cpp | 1 | 101 | 50 | 2 | 699 | 45 | 521 | 15 | 115 |
| kempecycle-stary.cpp | 1 | 123 | 54 | 3 | 739 | 46 | 879 | 82 | 187 |

tu je otazka, na kolko velmi to chceme robit, lebo to celkom isto spomaluje cas behu (mozno nie tak velmi ako volanie sat solverov)
cas sa ocakavane zhorsil asi 3-nasobne, pricom vysledky nie su zas o tolko lepsie

*este neviem to spustit lebo mi preteka stack vo wsl :((*

### upgrade
(potencialne) viac ziskam ak su farbenia rozne. teda 2 moznosti:
1. zakazdym ked sa pytam satsolveru, odomknu sa mi vsetky hrany, znova ich mozem prejst, mozno sa dozviem nieco nove
2. robim to stale s `MAX_REPETITIONS` ale pocitam si ako daleko od seba musia byt (napriklad: museli nastat aspon 4 kempeho prepnutia) - potom farbenie mozno bude dost ine

# todo
- mazak povedal ze treba to robit nejak inak ako satsolverom. ze to je pomale
- tiez povedal, nech to robim na viac jadrach naraz a nech nejak rozumne spravim vstup, lebo rozparsovat tie veci trva dlho
- kempecycle viem iterovat, ak vhodne pokombinujem kempeho cykly ktore mozu mat rozne farby tak viem najst nejaku novu kriticku hranu
- pozriet ci mam pre kazdy graf farbenie kde vysledok = 1