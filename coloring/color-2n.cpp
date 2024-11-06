#include <bits/stdc++.h>
#include "color-2n.h"
#include "../template.h"

using namespace std;

struct edge{
    int a, b, c;
    edge(int a, int b, int c) : a(a), b(b), c(c){}
};
ostream& operator<<(ostream& os, const edge& p){
    return os << "(" << p.a<<", "<<p.b<<", "<<p.c<<")";
}

const int max_colors = 3;
int pocet_vyskusanych;
bool edge_color(vector<vector<pair<int, int>>>& colors, vector<edge>&edges, int e, int c){
    pocet_vyskusanych++;
    edge t = edges[e];
    // dbg(e, t, c);
    for(auto i: colors[t.a]){
        if(edges[i.second].c == c)return false;
    }
    for(auto i: colors[t.b]){
        if(edges[i.second].c == c)return false;
    }
    return true;
}

bool color_edge(vector<vector<pair<int, int>>>& colors, vector<edge>&edges, int e){
    // dbg(e);
    if(e == sz(edges)) return true;
    for(int i = 0;i< max_colors;i++){
        if(edge_color(colors, edges, e, i)){
            edges[e].c = i;
            if(color_edge(colors, edges, e+1))return true;
        }
        edges[e].c = -1;
    }
    return false;
}

bool is_colorable(vector<vector<int>>& vg){
    pocet_vyskusanych = 0;
    map<pair<int, int>, int> ids;
    vector<edge> edges;
    vector<vector<pair<int, int>>> colors;

    for(int i = 0;i<sz(vg);i++){
        colors.push_back({});
        for(auto s : vg[i]){
            if(!ids.count({i, s})){
                ids[{i, s}] = sz(edges);
                ids[{s, i}] = sz(edges);
                edges.push_back({i, s, -1});
            }
            colors[i].push_back({s, ids[{i, s}]});
        }
    }
    if(color_edge(colors, edges, 0)){
        // dbg("vyskusal som", pocet_vyskusanych);
        return true;
    }
    else {
        // dbg("vyskusal som", pocet_vyskusanych);
        return false;
    }
}