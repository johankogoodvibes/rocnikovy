#include "critical_checker.h"
#include "../template.h"
#include "../zChaff/SAT.h"
#include <map>
#include <set>
#define NUM_COLORS 3

vector<vector<int>> *graph;
map<pair<int, int>, int> edges;
vector<vector<vector<int>>> clauses;
SAT_Manager mng;
int runs_count =0;
void registruj_hranu(int a, int b){
    if(a>b)swap(a, b);
    if(edges.count({a, b}))return;
    edges[{a, b}] = (int)edges.size();
}

int get_edge_id(int a, int b){
    if(a>b)swap(a, b);
    return edges[{a, b}];
}
int variable_id(int edgeid, int color){
    return edgeid*NUM_COLORS + color + 1;
}
int true_id(int var){
    return var*2;
}
int false_id(int var){
    return var*2+1;
}

void configure_edges(){
    for(auto [k, i]:edges){
        // dbg("edge ", k, "index", i);
        // dbg("variables", variable_id(i, 0), variable_id(i, 1), variable_id(i, 2), "exactly one should be true");
        vector<int> one_true = {
            true_id(variable_id(i, 0)),
            true_id(variable_id(i, 1)),
            true_id(variable_id(i, 2))
        };
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
        clauses[i].push_back(not12); //tuto zohladnujem ze farby su prave 3
    }
}
void different_colors(int a, int b){
    // dbg("edges", a, b, "should not be equal");
    for(int color = 0;color<NUM_COLORS;color++){
        // dbg("variables", variable_id(a, color), variable_id(b, color), "should not both be true");
        vector<int> clause = {
            false_id(variable_id(a, color)),
            false_id(variable_id(b, color))
        };
        // dbg("rules to satisfy", clause);
        clauses.back().push_back(clause);
    }
}

void create_critical_checker(vector<vector<int>>&g){
    graph = &g;
    for(int i = 0;i<(int)g.size();i++){
        for(auto j:g[i]){
            registruj_hranu(i, j);
        }
    }
    clauses.resize(edges.size()+1);

    configure_edges();

    for(int i = 0;i<(int)g.size();i++){
        vector<int> incident_edges;
        for(auto j:g[i]){
            int a= i, b = j;
            incident_edges.push_back(get_edge_id(a, b));
        }
        for(auto a:incident_edges){
            for(auto b:incident_edges){
                if(a==b)continue;
                different_colors(a, b);
            }
        }
    }
}

int get_edge_color(int a, int b){
    int id = get_edge_id(a, b);
    int color = -1;
    for(int i = 0;i<NUM_COLORS;i++){
        int akt = SAT_GetVarAsgnment(mng, variable_id(id, i));
        if(akt == 1) {
            if(color != -1)dbg("nieco sa pokazilo");
            color = i;
        }
    }
    return color;
}

set<int> ignored;
bool is_colorable(){
    runs_count++;
    mng = SAT_InitManager();
    SAT_SetNumVariables(mng, edges.size()*NUM_COLORS);

    for(int i = 0;i< (int)clauses.size();i++){
        if(ignored.count(i))continue;
        for(auto j: clauses[i]){
            int* clause = new int[j.size()];
            copy(j.begin(), j.end(), clause);
            SAT_AddClause(mng, clause, j.size(), 1);
        }
    }

    int status = SAT_Solve(mng);
    if(status == SATISFIABLE){
        return true;
    }
    else if(status==UNSATISFIABLE){
        return false;
    }
    else {
        cerr<<"zly status"<<endl;
        return false;
    }
}

void ignore_edge(int a, int b){
    // dbg("ignoring edge", make_pair(a, b));
    if(ignored.size()!=0){
        cerr<<"uz ignorujem hranu"<<endl;
        return;    
    }
    for(auto s : (*graph)[a]){
        // dbg(make_pair(a, s));
        ignored.insert(get_edge_id(a, s));
    }
    
    for(auto s : (*graph)[b]){
        // dbg(make_pair(b, s));
        ignored.insert(get_edge_id(b, s));
    }
    // dbg(ignored);
}

void unignore_edge(int a, int b){
    ignored.clear();
}

void delete_critical_checker(){
    ignored.clear();
    graph=NULL;
    edges.clear();
    clauses.clear();
    mng=NULL;
}

void print_stats(){
    cerr<<"sat solver runs: "<< runs_count<<endl;
}