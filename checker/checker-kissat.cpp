#include <map>
#include <set>

extern "C" {
#include "../kissat_compiled/kissat.h"
}
#include "../template.h"
#include "critical_checker.h"
#define NUM_COLORS 3

vector<vector<int>>* graph;
map<pair<int, int>, int> edges;
vector<pair<int, int>> rids;
vector<vector<vector<int>>> clauses;
vector<int> model_val, first_model_val;
int runs_count = 0;
void registruj_hranu(int a, int b) {
    if (a > b) swap(a, b);
    if (edges.count({a, b})) return;
    rids.push_back({a, b});
    edges[{a, b}] = (int)edges.size();
}

int get_edge_id(int a, int b) {
    if (a > b) swap(a, b);
    return edges[{a, b}];
}
pair<int, int> get_edge_endpoints(int id) {
    return rids[id];
}
int variable_id(int edgeid, int color) {
    return edgeid * NUM_COLORS + color + 1;
}
int true_id(int var) {
    return var;
}
int false_id(int var) {
    return -var;
}
int edgeid_from_varid(int id) {
    return (abs(id) - 1) / NUM_COLORS;
}
int color_from_varid(int id) {
    return (abs(id) - 1) % NUM_COLORS;
}
void configure_edges() {
    for (auto [k, i] : edges) {
        // dbg("edge ", k, "index", i);
        // dbg("variables", variable_id(i, 0), variable_id(i, 1), variable_id(i, 2), "exactly one should be true");
        vector<int> one_true = {
            true_id(variable_id(i, 0)),
            true_id(variable_id(i, 1)),
            true_id(variable_id(i, 2))};
        vector<int> not01 = {
            false_id(variable_id(i, 0)),
            false_id(variable_id(i, 1)),
        };
        vector<int> not12 = {
            false_id(variable_id(i, 2)),
            false_id(variable_id(i, 1)),
        };
        vector<int> not02 = {
            false_id(variable_id(i, 0)),
            false_id(variable_id(i, 2)),
        };
        // dbg("rules to satisfy", one_true, not01, not02, not12);
        clauses[i].push_back(one_true);
        clauses[i].push_back(not01);
        clauses[i].push_back(not02);
        clauses[i].push_back(not12);  // tuto zohladnujem ze farby su prave 3
    }
}
void different_colors(int a, int b) {
    // dbg("edges", a, b, "should not be equal");
    for (int color = 0; color < NUM_COLORS; color++) {
        // dbg("variables", variable_id(a, color), variable_id(b, color), "should not both be true");
        vector<int> clause = {
            false_id(variable_id(a, color)),
            false_id(variable_id(b, color))};
        // dbg("rules to satisfy", clause);
        clauses.back().push_back(clause);
    }
}
void restrict_edge_color(pair<int, int> e, int restricted_color) {
    clauses.back().push_back({false_id(variable_id(get_edge_id(e.first, e.second), restricted_color))});
}
void create_critical_checker(vector<vector<int>>& g, vector<vector<int>>& restricted_colorings) {
    runs_count = 0;
    model_val.clear();
    first_model_val.clear();
    graph = &g;
    for (int i = 0; i < (int)g.size(); i++) {
        for (auto j : g[i]) {
            registruj_hranu(i, j);
        }
    }
    clauses.resize(edges.size() + 1);  // posledne pole je na clauses pre rozdielnost farieb hran

    configure_edges();

    for (int i = 0; i < (int)g.size(); i++) {
        vector<int> incident_edges;
        for (auto j : g[i]) {
            int a = i, b = j;
            incident_edges.push_back(get_edge_id(a, b));
        }
        for (auto a : incident_edges) {
            for (auto b : incident_edges) {
                if (a == b) continue;
                different_colors(a, b);
            }
        }
    }
    clauses.push_back({});  // tu budu zablokovane farbenia
    for (auto i : restricted_colorings) {
        clauses.back().push_back(i);
    }
}

set<int> ignored;
set<int> main_ignored;
int main_ignored_id;

int get_edge_color(int a, int b) {
    if (get_edge_id(a, b) == main_ignored_id) return 0;
    if (ignored.count(get_edge_id(a, b)) != 0) {
        // dbg("incidentna hrana", a, b, "ignorujem", main_ignored);
        if (main_ignored.count(a)) swap(a, b);
        // dbg("chcem najst farby vrcholu", a);
        set<int> av = {1, 2, 3};
        for (auto i : (*graph)[a]) {
            if (i == b) continue;
            av.erase(get_edge_color(a, i));
        }
        // dbg("mozem pouzit len", av);
        return *av.begin();
    }
    int id = get_edge_id(a, b);
    int color = -1;
    for (int i = 0; i < NUM_COLORS; i++) {
        const int var = variable_id(id, i);
        const int akt = (var >= 0 && var < (int)model_val.size()) ? model_val[var] : 0;

        if (akt == 1) {
            if (color != -1) cerr << "nieco sa pokazilo, mam viac farieb " << a << ' ' << b << endl;
            color = i;
        }
    }
    if (color == -1) cerr << "nieco sa pokazilo, nemam ziadnu farbu " << a << ' ' << b << endl;
    return color + 1;
}

bool is_colorable(int seed) {
    runs_count++;

    kissat* solver = kissat_init();
    kissat_set_option(solver, "quiet", 1);

    const int num_vars = static_cast<int>(edges.size() * NUM_COLORS);

    kissat_reserve(solver, num_vars);

    // Add all non-ignored clauses (DIMACS style: each clause ends with 0)
    for (int i = 0; i < (int)clauses.size(); ++i) {
        if (ignored.count(i)) continue;
        for (const auto& j : clauses[i]) {
            for (int lit : j) kissat_add(solver, lit);
            kissat_add(solver, 0);  // terminate clause
        }
    }
    if (seed != 0) {
        kissat_set_option(solver, "seed", seed);
        kissat_set_option(solver, "target", 2);
        kissat_set_option(solver, "randec", 1);
        kissat_set_option(solver, "randecfocused", 1);
        kissat_set_option(solver, "randecstable", 1);
        kissat_set_option(solver, "rephase", 1);
        kissat_set_option(solver, "phasesaving", 0);
        kissat_set_option(solver, "forcephase", 1);
        kissat_set_option(solver, "walkinitially", 1);
    }
    int status = kissat_solve(solver);
    model_val.assign(num_vars + 1, 0);
    if (status == 10) {
        for (int v = 1; v <= num_vars; ++v) {
            model_val[v] = (kissat_value(solver, v) > 0) ? 1 : 0;
        }

        if (first_model_val.size() == 0) first_model_val = model_val;
        kissat_release(solver);
        return true;
    } else if (status == 20) {
        kissat_release(solver);
        return false;
    } else {
        std::cerr << "zly status" << std::endl;
        kissat_release(solver);
        return false;
    }
}
vector<pair<pair<int, int>, int>> get_first_coloring() {
    vector<pair<pair<int, int>, int>> res;
    for (int i = 0; i < (int)size(first_model_val); i++) {
        if (first_model_val[i]) {
            res.push_back({get_edge_endpoints(edgeid_from_varid(i)), color_from_varid(i) + 1});
        }
    }
    return res;
}
vector<int> disable_first_coloring_clause() {
    vector<int> res;
    for (int i = 0; i < (int)size(first_model_val); i++) {
        if (first_model_val[i]) {
            res.push_back(-i);
        }
    }
    return res;
}
void ignore_edge(int a, int b) {
    if (ignored.size() != 0) {
        cerr << "uz ignorujem hranu" << endl;
        return;
    }
    main_ignored = {a, b};
    // dbg(main_ignored);
    main_ignored_id = get_edge_id(a, b);
    for (auto s : (*graph)[a]) {
        ignored.insert(get_edge_id(a, s));
    }

    for (auto s : (*graph)[b]) {
        ignored.insert(get_edge_id(b, s));
    }
}

void unignore_edge(int a, int b) {
    ignored.clear();
    main_ignored.clear();
    main_ignored_id = -1;
}

void delete_critical_checker() {
    ignored.clear();
    graph = NULL;
    edges.clear();
    clauses.clear();
}

int get_runs() { return runs_count; }

void print_stats() {
    cerr << "sat solver runs: " << runs_count << endl;
}