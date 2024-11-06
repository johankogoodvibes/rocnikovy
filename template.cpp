#include<iostream>
#include <vector>
#include "template.h"

using namespace std;

void dbg_graph(vector<vector<int>> g){
    for(int i =0;i<sz(g);i++){
        dbg(g[i]);
    }
}
