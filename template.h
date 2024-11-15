#ifndef temp
#define temp
#include<iostream>
#include <vector>
using namespace std;

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
    ((cout<< args<<" |"), ...);
    cout<<endl;
}

void dbg_graph(vector<vector<int>> g);
#endif // temp
