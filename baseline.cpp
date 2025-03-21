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

int main() {  // toto je furt rovnake
    read_and_go();
}