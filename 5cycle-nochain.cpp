#include <iostream>
#include <vector>
#include <set>
#include <queue>
#include <chrono>
#include "checker/critical_checker.h"
#include "template.h"

using namespace std;

void dfs5(vector<vector<int>>&g, set<pair<int, int>>&edges, set<pair<int, int>> &solved, int v, int c, vector<int>& z){
    z.push_back(v);
    if(z.size() == 5){
        if(v != c){
            z.pop_back();
            return;
        }
        for(int i = 1;i< (int)z.size();i++){
            int x = z[i], y = z[i-1];
            if(x>y)swap(x, y);
            // dbg("kriticka je aj hrana", make_pair(x, y));
            // edges.erase({x, y});
            solved.insert({x, y});
        }
        z.pop_back();
        return;
    }
    for(int i = 0;i< (int)z.size()-1;i++)if(z[i] == v){
        z.pop_back();
        return;
    }
    for(auto i : g[v]){
        dfs5(g, edges, solved, i, c, z);
    }
    z.pop_back();
    return;
}

bool check_critical(vector<vector<int>>&g){
    set<pair<int, int>> edges;
    set<pair<int, int>> solved;
    create_critical_checker(g);
    for(int i = 0;i<(int)g.size();i++){
        for(auto s:g[i]){
            if(i<s) edges.insert({i, s});
        }
    }
    while(!edges.size()==0){
        auto [a, b] = *edges.begin();
        edges.erase({a, b});
        if(solved.count({a, b}))continue;
        ignore_edge(a, b);
        if(!is_colorable()){
            delete_critical_checker();
            return false;
        }
        solved.insert({a, b});
        vector<int> n;
        dfs5(g, edges, solved, a, b, n);
        unignore_edge(a, b);
    }
    delete_critical_checker();
    return true;
}

int main(){
    auto start = chrono::high_resolution_clock::now();
    string s;cin>>s;
    int count = 0;
    while(s == "graph"){
        count++;
        int n;cin>>n;
        vector<vector<int>> g(n);
        for(int i = 0;i<n;i++){
            int a, b, c;cin>>a>>b>>c;
            g[i].push_back(a);
            g[i].push_back(b);
            g[i].push_back(c);
        }
        if(check_critical(g)){
            cout<<"is critical"<<endl;
        }
        else cout<<"not critical"<<endl;
        cin>>s;
    }
    auto end = chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    print_stats();
    cerr<<endl<<"solved "<< count << " snarks, time: "<<elapsed.count()<<"s"<<endl;
}