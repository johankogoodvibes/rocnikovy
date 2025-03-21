#include <chrono>
#include <iostream>
#include <queue>
#include <set>
#include <vector>

#include "checker/critical_checker.h"
#include "template.h"

using namespace std;

int get_dest_color(vector<vector<int>>& g, int start, int color) {
    for (auto i : g[start])
        if (get_edge_color(start, i) == color) return i;

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
    if(start != realstart) res.push_back(start);
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
        if(solved.count({a, b}))continue; // todo chcem to spravit aby som optimalizoval aj pre tieto hrany ale potrebujem poznat farbenie bez pustania sat
        ignore_edge(a, b);
        if(!is_colorable()){
            delete_critical_checker();
            return false;
        }
        solved.insert({a, b});
        vector<int> n;

        int s1 = g[a][0] != b ? g[a][0] : g[a][1], s2 = g[b][0] != a ? g[b][0] : g[b][1];
        int c1 = get_edge_color(a, s1), c2 = get_edge_color(b, s2);


        if (c1 != c2) {
            vector<int> cyklusa = get_kempe_cycle(g, a, c1, c2), cyklusb = get_kempe_cycle(g, b, c2, c1);
            set<int> allcolors = {1, 2, 3};
            allcolors.erase(c1);
            allcolors.erase(c2);
            auto res = get_connections(g, cyklusa, cyklusb, *allcolors.begin());
            for (auto i : res) {
                solved.insert(i);
            }
        } else {
            for (int c2 = 1; c2 <= 3; c2++) {
                if (c2 == c1) continue;
                vector<int> cyklusa = get_kempe_cycle(g, a, c1, c2), cyklusb = get_kempe_cycle(g, b, c1, c2);
                set<int> allcolors = {1, 2, 3};
                allcolors.erase(c1);
                allcolors.erase(c2);
                auto res = get_connections(g, cyklusa, cyklusb, *allcolors.begin());
                for (auto i : res) {
                    solved.insert(i);
                }
            }
        }

        unignore_edge(a, b);
    }
    delete_critical_checker();
    return true;
}

int main() {  // toto je furt rovnake
    read_and_go();
}