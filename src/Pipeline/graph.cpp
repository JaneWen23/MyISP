#include "graph.hpp"
#include <iostream>
#include <unordered_set>


// bool is_subset(const std::vector<MODULE_NAME>& a, const std::vector<MODULE_NAME>& b){  // not yet in use
//     // check if a is subset of b (do not consider duplicate values)
//     std::unordered_set<MODULE_NAME> usetA; // unordered set from "a"
//     for (auto ia = a.begin(); ia != a.end(); ++ia){
//         if (usetA.count(*ia) == 0){
//             usetA.insert(*ia);
//         }
//     }
//     std::unordered_set<MODULE_NAME> usetB; // unordered set from "b"
//     for (auto ib = b.begin(); ib != b.end(); ++ib){
//         if (usetB.count(*ib) == 0){
//             usetB.insert(*ib);
//         }
//     }
//     // check if elements in a is also in b:
//     for (auto it = usetA.begin(); it != usetB.end(); ++it){
//         if (usetB.count(*it) == 0){
//             return false;
//         }
//     }
//     return true;
// }


bool is_graph_valid(const Graph_t& graph){
    // only check basic rules of a directed graph. does NOT guarantee to be acyclic.
    std::unordered_set<MODULE_NAME> definedVtx;
    for(auto it = graph.begin(); it != graph.end(); ++it){
        // check if there are duplicate vertices:
        if (definedVtx.count((*it).module) == 0){
            definedVtx.insert((*it).module);
        }
        else{
            std::cout<<"invalid graph: detected duplicate vertex: "<< get_module_name((*it).module)<<".\n";
            return false;
        }
    }

    for (int i = 0; i < graph.size(); ++i){
        if (graph[i].succModules.size() > 0){
            std::unordered_set<MODULE_NAME> succVtx;
            for (auto it = graph[i].succModules.begin(); it != graph[i].succModules.end(); ++it){
                // check if there are duplicate edges:
                if (succVtx.count(*it) == 0){
                    succVtx.insert((*it));
                }
                else{
                    std::cout<<"invalid graph: detected duplicate edge: from "<< get_module_name(graph[i].module) <<
                    " to "<< get_module_name(*it) <<".\n";
                    return false;
                }
            }

            // check if there exists vertex connected to itself:
            if (succVtx.count(graph[i].module) != 0){
                std::cout<<"invalid graph: detected an edge from and to the same vertex: " <<
                get_module_name(graph[i].module) <<".\n";
                return false;
            }

            // check if there is edge connecting to undefined vertex:
            for (auto it = succVtx.begin(); it != succVtx.end(); ++it){
                if (definedVtx.count(*it) != 1){
                    std::cout<<"invalid graph: detected an edge from "<< get_module_name(graph[i].module) <<
                    " connecting to "<< get_module_name(*it) <<", which is not defined.\n";
                    return false;
                }
            }
        }
    }

    return true;
}

void print_graph(Graph_t& graph){
    std::cout<<"graph:\n";
    for(auto it = graph.begin(); it != graph.end(); ++it){
        std::cout<< "node "<< get_module_name((*it).module)<<": ";
        int len = ((*it).succModules).size();
        if (len == 0){
            std::cout<< "has no successor; ";
        }
        else{
            std::cout<< "has successor(s): ";
            for (int i = 0; i < len; ++i){
                std::cout<< get_module_name((*it).succModules[i]) <<", ";
            }
        }
        std::cout<<"\n";
    }
}

const std::unordered_map<MODULE_NAME, int> make_module_vertex_map(const Graph_t& graph){
    std::unordered_map<MODULE_NAME, int> mvMap;
    for (int i = 0; i < graph.size(); ++i){
        mvMap.insert(std::pair<MODULE_NAME, int>(graph[i].module, i));
    }
    return mvMap;
}


typedef enum{
    UNVISITED,
    VISITING,
    VISITED
} VISIT_STATUS; // this is only for dfs topological sort

bool dfs_topsort(const MODULE_NAME u, const Graph_t& graph, VISIT_STATUS* vStatus, const std::unordered_map<MODULE_NAME, int>& mvMap, MODULE_NAME* sorted, int* ind){
    // topological sort vertices of directed acyclic graph, using DFS. this is not the only way.
    vStatus[mvMap.at(u)] = VISITING;
    Adjacency_t node = graph[mvMap.at(u)];
    for (int j = 0; j < node.succModules.size(); ++j){
        MODULE_NAME v = node.succModules[j];
        if (vStatus[mvMap.at(v)] == VISITING){
            // in this case, dfs tries to visit some predecessor. it means cycle exists, cannot topsort.
            return false;
        }
        if (vStatus[mvMap.at(v)] == UNVISITED){
            if ( ! dfs_topsort(v, graph, vStatus, mvMap, sorted, ind)){
                return false; // if the recursion returns false, the current "stack" should also return false.
                // if do not return false, it will go to the next lines and return true eventually.
            }
        }
    }

    // only if no "false", can the program go to here:
    vStatus[mvMap.at(u)] = VISITED;
    sorted[*ind] = u;
    (*ind)--;
    return true;
}

void topological_sort(const Graph_t& graph, MODULE_NAME* sorted){
    if ( ! is_graph_valid(graph)){
        std::cout<<"error: graph is invalid, cannot proceed.\n";
        exit(1);
    }

    const int n = graph.size();
    if (n < 1){
        std::cout<<"error: graph has no vertices, cannot sort. exited.\n";
        exit(1);
    }
    else if (n == 1){
        sorted[0] = graph[0].module;
        return;
    }

    std::unordered_map<MODULE_NAME, int> mvMap = make_module_vertex_map(graph);

    VISIT_STATUS vStatus[n];
    for (int i = 0; i < n; ++i){
        vStatus[i] = UNVISITED;
    }

    int ind = n-1;
    for (int i = 0; i < n; ++i){
        if(!vStatus[i]){
            MODULE_NAME u = graph[i].module;
            if ( ! dfs_topsort(u, graph, vStatus, mvMap, sorted, &ind)){
                std::cout<<"error: topological sort failed because it encounters cycle in graph. exited.\n";
                exit(1);
            }
        }
    }
}

void print_sorted_nodes(const MODULE_NAME* sorted, const int len){
    std::cout<<"sorted:\n";
    for (int i = 0; i < len - 1; ++i){
        std::cout<< get_module_name(sorted[i])<<" --> ";
    }
    std::cout<< get_module_name(sorted[len-1]);
    std::cout<<"\n";
}

const int find_ind_in_sorted(const MODULE_NAME m, const MODULE_NAME* sorted, const int len){
    for (int i = 0; i < len; ++i){
        if (sorted[i] == m){
            return i;
        }
    }
    return 0;
}


void test_graph(){
    int n = 9; // number of nodes

    Graph_t graph(n);
    
    graph[0] = {DUMMY0, {DUMMY1, DUMMY2}}; // the directed edges are implicitly shown as from DUMMY0 to DUMMY1, and from DUMMY0 to DUMMY2
    graph[1] = {DUMMY1, {DUMMY3}}; // the directed edges are implicitly shown as from DUMMY1 to DUMMY3
    graph[2] = {DUMMY2, {DUMMY3}}; // the directed edges are implicitly shown as from DUMMY2 to DUMMY3
    graph[3] = {DUMMY3, {DUMMY4, DUMMY5}}; // and so on ...
    graph[4] = {DUMMY4, {DUMMY6}};
    graph[5] = {DUMMY5, {DUMMY6}};
    graph[6] = {DUMMY6, {DUMMY8}};
    graph[7] = {DUMMY7, {DUMMY6}}; 
    graph[8] = {DUMMY8, {}};

    print_graph(graph);

    MODULE_NAME sorted[n];
    topological_sort(graph, sorted);
    print_sorted_nodes(sorted, n);
}