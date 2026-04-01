#ifndef critcheck
#define critcheck
#include <vector>

void create_critical_checker(std::vector<std::vector<int>>& vg, std::vector<std::vector<int>>& restricted_colorings);
void delete_critical_checker();
bool is_colorable(int seed = 0);
void ignore_edge(int a, int b);
void unignore_edge(int a, int b);
int get_edge_color(int a, int b);
std::vector<std::pair<std::pair<int, int>, int>> get_first_coloring();
std::vector<int> disable_first_coloring_clause();
// void restrict_edge_color(pair<int, int> e, int restricted_color);
void print_stats();
int get_runs();
#endif  // critcheck