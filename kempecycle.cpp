#include <algorithm>
#include <chrono>
#include <iostream>
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

void solve_kempecycle(vector<vector<int>>& g, set<pair<int, int>>& solved, int a, int b);

void solve_for_cycles(vector<vector<int>>& g, set<pair<int, int>>& solved, int a, int b, vector<int>& cyklusa, vector<int>& cyklusb, int c1, int c2) {
    set<int> allcolors = {1, 2, 3};
    allcolors.erase(c1);
    allcolors.erase(c2);
    int c3 = *allcolors.begin();

    auto res = get_connections(g, cyklusa, cyklusb, c3);
    for (auto i : res) {
        vector<int> kempe_switch_cycle = {};
        int start = 0;
        while (cyklusa[start] != i.first && cyklusa[start] != i.second) {
            kempe_switch_cycle.push_back(cyklusa[start]);
            start++;
        }

        if (i.first != cyklusa[start]) swap(i.first, i.second);
        kempe_switch_cycle.push_back(i.first);
        kempe_switch_cycle.push_back(i.second);

        vector<int> opacne = {};
        start = 0;
        while (cyklusb[start] != i.second) {
            opacne.push_back(cyklusb[start]);
            start++;
        }
        reverse(opacne.begin(), opacne.end());
        for (auto i : opacne) kempe_switch_cycle.push_back(i);
        kempe_switch_cycle.push_back(b);
        kempe_switch_cycle.push_back(a);

        int prev = a;
        map<pair<int, int>, int> changed;
        for (auto i : kempe_switch_cycle) {
            changed[{prev, i}] = get_current_edge_color(prev, i);
            edgecolors[{prev, i}] = edgecolors[{i, prev}] = get_current_edge_color(prev, i) ^ c3;
            prev = i;
        }
        solve_kempecycle(g, solved, i.first, i.second);

        for (auto i : changed) {
            edgecolors[i.first] = edgecolors[{i.first.second, i.first.first}] = i.second;
        }
    }
}

void solve_kempecycle(vector<vector<int>>& g, set<pair<int, int>>& solved, int a, int b) {
    if (solved.count({a, b})) return;
    solved.insert({a, b});

    int s1 = g[a][0] != b ? g[a][0] : g[a][1], s2 = g[b][0] != a ? g[b][0] : g[b][1];
    int c1 = get_current_edge_color(a, s1), c2 = get_current_edge_color(b, s2);
    if (c1 != c2) {
        vector<int> cyklusa = get_kempe_cycle(g, a, c1, c2), cyklusb = get_kempe_cycle(g, b, c2, c1);
        solve_for_cycles(g, solved, a, b, cyklusa, cyklusb, c1, c2);
    } else {
        for (int c2 = 1; c2 <= 3; c2++) {
            if(c2 == c1) continue;
            vector<int> cyklusa = get_kempe_cycle(g, a, c1, c2), cyklusb = get_kempe_cycle(g, b, c1, c2);
            solve_for_cycles(g, solved, a, b, cyklusa, cyklusb, c1, c2);
        }
    }
}

bool check_critical(vector<vector<int>>& g) {
    set<pair<int, int>> edges;
    set<pair<int, int>> solved;
    create_critical_checker(g);
    for (int i = 0; i < (int)g.size(); i++) {
        for (auto s : g[i]) {
            if (i < s) edges.insert({i, s});
        }
    }
    while (!edges.size() == 0) {
        auto [a, b] = *edges.begin();
        edges.erase({a, b});
        if (solved.count({a, b})) continue;
        ignore_edge(a, b);

        if (!is_colorable()) {
            delete_critical_checker();
            return false;
        }
        for (int i = 0; i < (int)g.size(); i++) {
            for (auto j : g[i]) {
                edgecolors[{i, j}] = get_edge_color(i, j);
            }
        }
        
        solve_kempecycle(g, solved, a, b);

        unignore_edge(a, b);
    }
    delete_critical_checker();
    return true;
}

int main() {  // toto je furt rovnake
    auto start = chrono::high_resolution_clock::now();
    string s;
    cin >> s;
    int count = 0;
    while (s == "graph") {
        count++;
        int n;
        cin >> n;
        vector<vector<int>> g(n);
        for (int i = 0; i < n; i++) {
            int a, b, c;
            cin >> a >> b >> c;
            g[i].push_back(a);
            g[i].push_back(b);
            g[i].push_back(c);
        }
        if (check_critical(g)) {
            cout << "is critical" << endl;
        } else
            cout << "not critical" << endl;
        cin >> s;
    }
    auto end = chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    print_stats();
    cerr << endl
         << "solved " << count << " snarks, time: " << elapsed.count() << "s" << endl;
}