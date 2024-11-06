#include <iostream>
#include <vector>
#include "coloring/color-2n.h"
#include "template.h"

using namespace std;

vector<vector<int>> vyhod(vector<vector<int>> g, int v){
    vector<vector<int>> gc;
    for(int i = 0;i< sz(g);i++){
        if(i == v)continue;
        gc.push_back({});
        for(auto j:g[i]){
            if(j!=v){
                if(j>v)j--;
                gc.back().push_back(j);
            }
        }
    }
    return gc;
}

bool check_critical(vector<vector<int>>&g){
    vector<pair<int, int>> edges;
    for(int i = 0;i<sz(g);i++){
        for(auto s:g[i]){
            if(i<s) edges.push_back({i, s});
        }
    }
    for(auto [a, b]:edges){
        // cerr<<"kontrolujem "<<a<<' '<<b<<endl;
        // dbg("graf g");
        // dbg_graph(g);
        auto nb = vyhod(g, b);
        // dbg("graf g - b");
        // dbg_graph(nb);
        auto na = vyhod(nb, a);
        // dbg("graf g-b-a");
        // dbg_graph(na);
        if(!is_colorable(na)){
            return false;
        }
        else {
            // cerr<< "hrana "<< a<<' '<<b<<" je kriticka\n";
        }
    }
    return true;
}


int main(){
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
    cout<<count<<endl;
}