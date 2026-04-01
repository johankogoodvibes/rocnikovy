#ifndef temp
#define temp
#include <chrono>
#include <iostream>
#include <vector>

#include "checker/critical_checker.h"
using namespace std;

// toto niekedy potom urobim
bool check_critical(vector<vector<int>>& g, bool vsetky, vector<vector<int>>& restricted_colorings);

template <class A, class B>
ostream& operator<<(ostream& os, const pair<A, B>& p) {
    return os << "(" << p.first << ", " << p.second << ")";
}

template <class A, class B, class C>
basic_ostream<A, B>& operator<<(basic_ostream<A, B>& os, const C& c) {
    for (auto it = begin(c); it != end(c); ++it)
        os << (it == begin(c) ? "" : " ") << *it;
    return os;
}

template <typename... Args>
void dbg(Args&&... args) {
    ((cerr << args << " |"), ...);
    cerr << endl;
}

void dbg_graph(vector<vector<int>>& g);
const int MAX_REPS = 1234567890;

inline void read_and_go(int argc = 0, char* argv[] = NULL) {
    bool DBG = false;
    bool ALL = false;
    bool SINGLE = false;
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--dbg") { // flag na zapnutie debugov
            DBG = true;
        }
        if (arg == "--all") { // vsetky musia byt kriticke
            ALL = true;
        }
        if (arg == "--single") { // chcem najst farbenie co to da na jeden sup
            SINGLE = true;
        }
    }
    auto start = chrono::high_resolution_clock::now();
    string s;
    cin >> s;
    int count = 0;
    int total_runs = 0, total_reps = 0;
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

        int reps = 0;
        vector<vector<int>> restricted_colorings;
        while (reps++ < MAX_REPS) {
            cerr << "Solving " << count << "th graph:       ";
            if (check_critical(g, ALL, restricted_colorings)) {
                cout << "is critical" << endl;
            } else {
                cout << "not critical" << endl;
            }

            int aktruns = get_runs();
            cerr << " - " << aktruns << " sat solver runs" << endl;
            if (!SINGLE) break;
            if (aktruns == 1) {
                cerr << "found correct coloring on " << reps << "th rep" << endl;
                total_reps += reps;
                break;
            } else {
                cerr << "bad coloring on " << reps << "th rep" << endl;
                restricted_colorings.push_back(disable_first_coloring_clause());
            }
        }
        total_runs += get_runs();
        cin >> s;
    }
    auto end = chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    cerr << "Total reps: " << total_reps << endl;
    cerr << "sat solver runs: " << total_runs << endl;
    // print_stats();
    if (DBG) {
        cerr << endl
             << "solved " << count << " snarks, time: " << elapsed.count() << "s" << endl;
    }
}
#endif  // temp
