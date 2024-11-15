#ifndef critcheck
#define critcheck
#include <vector>

void create_critical_checker(std::vector<std::vector<int>>&vg);
void delete_critical_checker();
bool is_colorable();
void ignore_edge(int a, int b);
void unignore_edge(int a, int b);
int get_edge_color(int a, int b);

#endif // critcheck