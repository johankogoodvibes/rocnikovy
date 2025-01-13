#include "critical_checker.h"
#include "../template.h"
#include <map>
//
struct edge{
    int a, b, c;
    edge(int a, int b, int c) : a(a), b(b), c(c){}
};
ostream& operator<<(ostream& os, const edge& p){
    return os << "(" << p.a<<", "<<p.b<<", "<<p.c<<")";
}


vector<vector<int>>* g;
const int max_colors = 3;
int pocet_vyskusanych = 0;
vector<int> ignored;
vector<edge> final_state;
map<pair<int, int>, int> *current_ids;


int get_edge_color(int a, int b){
    if(final_state.empty())return -1;
    int id = (*current_ids)[{a, b}];
    edge t = final_state[id];
    return t.c;
}

void log(){
    cout<<"log graph"<<endl;
    for(edge i : final_state){
        cout<< i<<endl;
    }
    cout<<endl;
}

bool can_have_color(vector<vector<pair<int, int>>>& colors, vector<edge>&edges, int e, int c){
    // pocet_vyskusanych++;
    edge t = edges[e];
    for(auto i: colors[t.a]){
        if(edges[i.second].c == c)return false;
    }
    for(auto i: colors[t.b]){
        if(edges[i.second].c == c)return false;
    }
    return true;
}

bool color_edge(vector<vector<pair<int, int>>>& colors, vector<edge>&edges, int e){
    if(e == (int) edges.size()) {
        final_state = edges;
        return true;
    }
    
    {
        auto t = edges[e];
        for(auto i:ignored){
            if(i==t.a||i==t.b){
                // dbg("ignorujem hranu", t);
                if(color_edge(colors, edges, e+1))return true;
                return false;
            }
        }
    }
    // dbg(edges[e]);
    // log();
    for(int i = 0;i< max_colors;i++){
        if(can_have_color(colors, edges, e, i)){
            edges[e].c = i;
            if(color_edge(colors, edges, e+1))return true;
        }
        edges[e].c = -1;
    }
    return false;
}

void create_critical_checker(vector<vector<int>>& vg){
    pocet_vyskusanych = 0;
    g=&vg;
}
void delete_critical_checker(){
    g = NULL;
    pocet_vyskusanych =0;
    ignored.clear();
    final_state.clear();
}
bool is_colorable(){
    map<pair<int, int>, int> ids;
    vector<edge> edges;
    vector<vector<pair<int, int>>> colors;

    for(int i = 0;i<(int)g->size();i++){
        colors.push_back({});
        for(auto s : (*g)[i]){
            if(!ids.count({i, s})){
                ids[{i, s}] = (int) edges.size();
                ids[{s, i}] = (int) edges.size();
                edges.push_back({i, s, -1});
            }
            colors[i].push_back({s, ids[{i, s}]});
        }
    }
    current_ids = &ids;
    bool colorable = color_edge(colors, edges, 0);
    return colorable;
}
void ignore_edge(int a, int b){
    if(ignored.size()!=0) cerr<<"uz ignorujem nejaku hranu"<<endl;
    else ignored ={a, b};
}
void unignore_edge(int a, int b){
    ignored.clear();
}