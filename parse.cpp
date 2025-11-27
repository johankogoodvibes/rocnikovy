#include <bits/stdc++.h>

using namespace std;

int main(int argc, char* argv[]){
    int count;
    count = atoi(argv[1]);
    if(count == -1){
        cerr << "parsujem vsetky" << endl;
    }
    else cerr<<"parsujem " << count<<endl;
    string s; cin>>s;
    while(s != "koniec"&& count--!=0){
        cout<<"graph"<<endl;
        string dummy; cin>>dummy>>dummy;
        
        int n;cin>>dummy;dummy.pop_back();
        n = stoi(dummy);
        vector<vector<int>> g(n);
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
            }
        }
        cout<< n<<endl;
        for(auto i: g)cout<<i[0]<<' '<<i[1]<<' '<<i[2]<<endl;
        cout<<endl;
        cin>>s;
    }
    cout<<"koniec"<<endl;
}