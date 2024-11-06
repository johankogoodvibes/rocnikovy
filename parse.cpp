#include <bits/stdc++.h>

using namespace std;

int main(){
    string s; cin>>s;
    while(s != "koniec"){
        cout<<"graph"<<endl;
        string dummy; cin>>dummy>>dummy;
        
        int n;cin>>dummy;dummy.pop_back();
        n = stoi(dummy);
        vector<vector<int>> g(n);
        vector<pair<int, int>> ans;
        for(int i = 0;i< n;i++){
            cin>>dummy>>dummy;
            bool koniec = false;
            while(!koniec){
                int sus;
                cin>>dummy;
                if(dummy.back()==';'){
                    dummy.pop_back();
                    koniec = true;
                }
                sus = stoi(dummy);
                g[i].push_back(sus);
                if(i< sus)ans.push_back({i, sus});
            }
        }
        cout<< n<<' '<<ans.size()<<endl;
        for(auto i: ans)cout<< i.first<<' '<<i.second<<endl;
        cout<<endl;
        cin>>s;
    }
    cout<<"koniec"<<endl;
}