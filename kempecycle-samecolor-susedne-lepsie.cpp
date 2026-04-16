#include <algorithm>
#include <fstream>
#include <map>
#include <queue>
#include <set>
#include <vector>

#include "checker/critical_checker.h"
#include "template.h"

map<pair<int, int>, int> edgecolors;
int get_current_edge_color(int a, int b) {
    return edgecolors[{a, b}];
}
void dbg_graph(vector<vector<int>>& g) {
    dbg("vypisujem graf");
    for (int i = 0; i < (int)g.size(); i++) {
        for (auto j : g[i]) {
            dbg(i, j, get_current_edge_color(i, j));
        }
    }
    dbg("koniec");
}

int get_dest_color(vector<vector<int>>& g, int start, int color) {
    for (auto i : g[start])
        if (get_current_edge_color(start, i) == color) return i;

    cerr << "zle je, nemam hladanu farbu " << start << ' ' << color << endl;
    return -1;
}

vector<int> get_kempe_cycle(vector<vector<int>>& g, int start, int akt_color, int next_color, int realstart = -1) {
    if (start == realstart) {
        return {};
    }
    if (realstart == -1) realstart = start;
    int next = get_dest_color(g, start, akt_color);
    auto res = get_kempe_cycle(g, next, next_color, akt_color, realstart);
    if (start != realstart) res.push_back(start);
    return res;
}

vector<pair<int, int>> get_connections(vector<vector<int>>& g, vector<int> cycle1, vector<int> cycle2, int color) {
    set<int> cycle2_set;
    for (auto i : cycle2) cycle2_set.insert(i);
    vector<pair<int, int>> connections;
    for (auto i : cycle1) {
        int s = get_dest_color(g, i, color);
        if (cycle2_set.count(s) != 0) {
            connections.push_back({i, s});
        }
    }
    return connections;
}
int getLastColor(int c1, int c2) {
    set<int> allcolors = {1, 2, 3};
    allcolors.erase(c1);
    allcolors.erase(c2);
    int c3 = *allcolors.begin();
    return c3;
}

void apply_kempeswitch(vector<int>& cycle, int c) {
    int prev = cycle.back();
    for (auto i : cycle) {
        edgecolors[{prev, i}] = edgecolors[{i, prev}] = get_current_edge_color(prev, i) ^ c;
        prev = i;
    }
}
void solve_kempecycle(vector<vector<int>>& g, set<pair<int, int>>& solved, int a, int b);

void solve_for_cycles(vector<vector<int>>& g, set<pair<int, int>>& solved, int a, int b, vector<int>& cyklusa, vector<int>& cyklusb, int c1, int c2) {
    int c3 = getLastColor(c1, c2);
    auto res = get_connections(g, cyklusa, cyklusb, c3);
    for (auto i : res) {
        if (solved.count({min(i.first, i.second), max(i.first, i.second)})) {
            continue;
        }
        vector<int> interesting_cycle = {};
        int start = 0;
        while (cyklusa[start] != i.first && cyklusa[start] != i.second) {
            interesting_cycle.push_back(cyklusa[start]);
            start++;
        }

        if (i.first != cyklusa[start]) swap(i.first, i.second);
        interesting_cycle.push_back(i.first);
        interesting_cycle.push_back(i.second);

        vector<int> opacne = {};
        start = 0;
        while (cyklusb[start] != i.second) {
            opacne.push_back(cyklusb[start]);
            start++;
        }
        reverse(opacne.begin(), opacne.end());
        for (auto i : opacne) interesting_cycle.push_back(i);
        interesting_cycle.push_back(b);
        interesting_cycle.push_back(a);

        apply_kempeswitch(interesting_cycle, c3);
        solve_kempecycle(g, solved, i.first, i.second);
        apply_kempeswitch(interesting_cycle, c3);
    } 
}

void solve_susedne(vector<vector<int>>& g, set<pair<int, int>>& solved, int a, int b) {
    // len pozriem ci viem vyriesit susedne hrany
    int s1 = g[a][0] != b ? g[a][0] : g[a][1], s2 = g[b][0] != a ? g[b][0] : g[b][1];
    int c1 = get_current_edge_color(a, s1), c2 = get_current_edge_color(b, s2);
    // zistim si farby susedov
    int c3 = getLastColor(c1, c2);
    if (c1 != c2) {
        {
            vector<int> cyklusa = get_kempe_cycle(g, a, c1, c3), cyklusb = {b};
            solve_for_cycles(g, solved, a, b, cyklusa, cyklusb, c1, c3);
        }
        {
            vector<int> cyklusa = {a}, cyklusb = get_kempe_cycle(g, b, c2, c3);
            solve_for_cycles(g, solved, a, b, cyklusa, cyklusb, c2, c3);
        }
    } else {
        // su rovnake
        for (int c2 = 1; c2 <= 3; c2++) {
            if (c2 == c1) continue;
            c3 = getLastColor(c1, c2);
            vector<int> k = get_kempe_cycle(g, b, c1, c2);
            k.push_back(b);
            apply_kempeswitch(k, c3);
            {
                vector<int> cyklusa = get_kempe_cycle(g, a, c1, c3), cyklusb = {b};
                solve_for_cycles(g, solved, a, b, cyklusa, cyklusb, c1, c3);
            }
            {
                vector<int> cyklusa = {a}, cyklusb = get_kempe_cycle(g, b, c2, c3);
                solve_for_cycles(g, solved, a, b, cyklusa, cyklusb, c2, c3);
            }
            apply_kempeswitch(k, c3);

            k = get_kempe_cycle(g, a, c1, c2);
            k.push_back(a);
            apply_kempeswitch(k, c3);
            {
                vector<int> cyklusa = get_kempe_cycle(g, a, c2, c3), cyklusb = {b};
                solve_for_cycles(g, solved, a, b, cyklusa, cyklusb, c2, c3);
            }
            {
                vector<int> cyklusa = {a}, cyklusb = get_kempe_cycle(g, b, c1, c3);
                solve_for_cycles(g, solved, a, b, cyklusa, cyklusb, c1, c3);
            }
            apply_kempeswitch(k, c3);
        }
    }
}

void solve_kempecycle(vector<vector<int>>& g, set<pair<int, int>>& solved, int a, int b) {
    if (a > b) swap(a, b);
    if (solved.count({a, b})) return;
    solved.insert({a, b});

    int s1 = g[a][0] != b ? g[a][0] : g[a][1], s2 = g[b][0] != a ? g[b][0] : g[b][1];
    int c1 = get_current_edge_color(a, s1), c2 = get_current_edge_color(b, s2);
    vector<int> vyrovnat;
    int vyrovnatc = -1;

    solve_susedne(g, solved, a, b);

    if (c1 != c2) {
        vyrovnatc = getLastColor(c1, c2);
        vyrovnat = get_kempe_cycle(g, b, c2, c1);
        vyrovnat.push_back(b);
        apply_kempeswitch(vyrovnat, vyrovnatc);
    }
    for (int c2 = 1; c2 <= 3; c2++) {
        if (c2 == c1) continue;
        vector<int> cyklusa = get_kempe_cycle(g, a, c1, c2),
                    cyklusb = get_kempe_cycle(g, b, c1, c2);

        solve_for_cycles(g, solved, a, b, cyklusa, cyklusb, c1, c2);
    }
    if (!vyrovnat.empty()) apply_kempeswitch(vyrovnat, vyrovnatc);
}

ofstream vyfarbenie1("vyfarbenie.txt");
void vyfarbi(vector<vector<int>>& g) {
    vyfarbenie1 << "dbg graph" << endl
                << endl;
    vyfarbenie1 << g.size() << endl
                << g.size() * 3 / 2 << endl;
    for (auto [a, b] : edgecolors) {
        if (a.first < a.second)
            vyfarbenie1 << a.first << ' ' << a.second << ' ' << b << "\n";
    }
    vyfarbenie1 << "koniec" << endl
                << endl;
}

bool check_critical(vector<vector<int>>& g, bool vsetky, vector<vector<int>>& restricted_colorings) {
    set<pair<int, int>> edges;
    set<pair<int, int>> solved;
    create_critical_checker(g, restricted_colorings);
    for (int i = 0; i < (int)g.size(); i++) {
        for (auto s : g[i]) {
            if (i < s) edges.insert({i, s});
        }
    }
    bool ok = true;
    int runs = 0;
    while (!edges.size() == 0) {
        auto [a, b] = *edges.begin();
        edges.erase({a, b});
        if (solved.count({a, b})) continue;
        ignore_edge(a, b);

        runs++;
        if (!is_colorable()) {
            if (vsetky) {
                if (ok) cerr << "- has answer: " << runs << " runs ";
                ok = false;
                unignore_edge(a, b);
                continue;
            }
            delete_critical_checker();
            return false;
        }
        for (int i = 0; i < (int)g.size(); i++) {
            for (auto j : g[i]) {
                edgecolors[{i, j}] = get_edge_color(i, j);
            }
        }
        vyfarbi(g);
        solve_kempecycle(g, solved, a, b);

        unignore_edge(a, b);
    }
    delete_critical_checker();
    return ok;
}

int main(int argc, char* argv[]) {  // toto je furt rovnake
    read_and_go(argc, argv);
}