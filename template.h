#ifndef temp
#define temp
#include <chrono>
#include <iostream>
#include <vector>
using namespace std;

// toto niekedy potom urobim
bool check_critical(vector<vector<int>>& g, bool vsetky, int seed);
void print_stats();
int get_runs();

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

inline void read_and_go(int argc = 0, char* argv[] = NULL) {
    bool DBG = false;
    bool ALL = false;
    bool SINGLE = false;
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--dbg") {
            DBG = true;
        }
        if (arg == "--all") {
            ALL = true;
        }
        if (arg == "--single") {
            SINGLE = true;
        }
    }
    auto start = chrono::high_resolution_clock::now();
    string s;
    cin >> s;
    int count = 0;
    int prevruns = 0;
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

        int seed = 0;
        while (true) {
            cerr << "Solving " << count << "th graph:       ";
            if (check_critical(g, ALL, seed)) {
                cout << "is critical" << endl;
            } else {
                cout << "not critical" << endl;
            }

            int aktruns = get_runs();
            int now = aktruns - prevruns;
            cerr << " - " << now << " sat solver runs" << endl;
            prevruns = aktruns;
            if (!SINGLE) break;
            if (now == 1) {
                cerr << "found correct seed: " << seed << endl;
                break;
            } else {
                cerr << "bad seed: " << seed << endl;
            }
            seed++;
        }

        cin >> s;
    }
    auto end = chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    print_stats();
    if (DBG) {
        cerr << endl
             << "solved " << count << " snarks, time: " << elapsed.count() << "s" << endl;
    }
}
#endif  // temp
