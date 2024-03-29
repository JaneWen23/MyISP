#ifndef _GRAPH_H
#define _GRAPH_H

#include "../Modules/COMMON/common.hpp"


typedef struct{
    MODULE_NAME module;
    std::vector<MODULE_NAME> succModules; // successor modules; a module is a "name" of a vertex
} Adjacency_t; // minimal definition for people to write in TOML.

typedef std::vector<Adjacency_t> Graph_t; // TOML


void topological_sort(const Graph_t& graph, MODULE_NAME* sorted);
void print_sorted_nodes(const MODULE_NAME* sorted, const int len);
void print_graph(Graph_t& graph);

// helper functions:
const std::unordered_map<MODULE_NAME, int> make_module_vertex_map(const Graph_t& graph);
const int find_ind_in_sorted(const MODULE_NAME m, const MODULE_NAME* sorted, const int len);

void test_graph();

#endif