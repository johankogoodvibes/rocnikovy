#include <iostream>
#include <vector>
#include <chrono>
#include "checker/critical_checker.h"
#include "template.h"

using namespace std;


bool check_critical(vector<vector<int>>&g){
    create_critical_checker(g);
    vector<pair<int, int>> edges;
    for(int i = 0;i<(int)g.size();i++){
        for(auto s:g[i]){
            if(i<s) edges.push_back({i, s});
        }
    }
    for(auto [a, b]:edges){
        ignore_edge(a, b);
        if(!is_colorable()){
            delete_critical_checker();
            return false;
        }
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
        if(count%100 == 0)cout<<count<<endl;
        int n, m;cin>>n>>m;
        vector<vector<int>> g(n);
        for(int i = 0;i<m;i++){
            int a, b;cin>>a>>b;
            if(b<a)swap(b, a);
            g[a].push_back(b);
            g[b].push_back(a);
        }
        if(check_critical(g)){
            cout<<"is critical"<<endl;
        }
        else cout<<"not critical"<<endl;
        cin>>s;
    }
    auto end = chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    cerr<<endl<<"solved "<< count << " snarks, time: "<<elapsed.count()<<"s"<<endl;
}