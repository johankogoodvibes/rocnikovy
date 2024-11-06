#ifndef temp
#define temp
#include<iostream>
#include <vector>
using namespace std;

#define For(i, a, n) for(ll i =a;i<n;i++)
#define vec vector
#define inf 1234567890
#define PI 3.14159265
#define mp make_pair
#define mod 1000000007
#define all(x) begin(x), end(x)
#define sz(x) (int) size(x)
#define del(x) x.clear();x.shrink_to_fit();
#define endl '\n'

template<class A, class B>
ostream& operator<<(ostream& os, const pair<A, B>& p){
    return os << "(" << p.first<<", "<<p.second<<")";
}

template<class A, class B, class C>
basic_ostream<A, B>& operator<< (basic_ostream<A, B>& os, const C& c){
    for(auto it=begin(c); it!= end(c); ++it)
        os<< (it==begin(c)? "":" ")<<*it;
    return os;
}

template<typename... Args>
void dbg(Args&&... args){
    ((cerr<< args<<" |"), ...);
    cerr<<endl;
}

void dbg_graph(vector<vector<int>> g);
#endif // temp
