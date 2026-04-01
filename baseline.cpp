#include <chrono>
#include <iostream>
#include <vector>

#include "checker/critical_checker.h"
#include "template.h"
using namespace std;

bool check_critical(vector<vector<int>>& g, bool vsetky, vector<vector<int>>& restricted_colorings) {
    create_critical_checker(g, restricted_colorings);
    vector<pair<int, int>> edges;
    for (int i = 0; i < (int)g.size(); i++) {
        for (auto s : g[i]) {
            if (i < s) edges.push_back({i, s});
        }
    }
    int runs = 0;
    bool ok = true;
    for (auto [a, b] : edges) {
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
        unignore_edge(a, b);
    }
    delete_critical_checker();
    return ok;
}

int main(int argc, char* argv[]) {  // toto je furt rovnake
    read_and_go(argc, argv);
}