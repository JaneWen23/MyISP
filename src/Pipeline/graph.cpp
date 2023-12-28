#include "graph.hpp"
#include <iostream>

bool is_subset(MODULE_NAME* a, int l_a, MODULE_NAME* b, int l_b){
    // check if a is subset of b.
    // this is naive way, for convenience.
    for (int i = 0; i < l_a; ++i){
        for (int j = 0; j < l_b; ++j){
            if (b[j] == a[i]){
                return true;
            }
        }
    }
    return false;
}

bool is_graph_valid(const Graph_t& graph){
    // only check basic rules of a directed graph. does NOT guarantee to be acyclic.
    const int n = graph.size();
    MODULE_NAME definedVtx[n]; // variable-sized, should not initialize.
    for (int i = 0; i < n; ++i){
        definedVtx[i] = graph[i].module;
    }
    
    // check if there are repeated vertices:
    std::sort(definedVtx, definedVtx + n);
    for (int i = 1; i < n; ++i){
        if (definedVtx[i-1] == definedVtx[i]){
            std::cout<<"invalid graph: there are repeated vertices.\n";
            return false;
        }
    }

    for (int i = 0; i < n; ++i){
        int l = graph[i].succ_modules.size();
        if (l > 0){
            MODULE_NAME adj[l];
            for (int j = 0; j < l; ++j){
                adj[j] = graph[i].succ_modules[j];
                // check if there is edge from and to the same vertex:
                if (graph[i].module == adj[j]){
                    std::cout<<"invalid graph: there exists an edge from and to the same vertex.\n";
                    return false;
                }
            }
            // check if there are repeated edges:
            std::sort(adj, adj + l);
            for (int k = 1; k < l; ++k){
                if (adj[k-1] == adj[k]){
                    std::cout<<"invalid graph: there are repeated edges.\n";
                    return false;
                }
            }
            // check if there is edge connecting to undefined vertex:
            // i.e. all elements in adj[] should be in definedVtx[].
            if ( ! is_subset(adj, l, definedVtx, n)){
                std::cout<<"invalid graph: there exists an edge connecting to undefined vertex.\n";
                return false;
            }
        }
    }

    return true;
}

void print_graph(Graph_t& graph){
    std::cout<<"graph:\n";
    for(auto it = graph.begin(); it != graph.end(); ++it){
        std::cout<< "node "<< get_module_name((*it).module)<<": ";
        int len = ((*it).succ_modules).size();
        if (len == 0){
            std::cout<< "has no successor; ";
        }
        else{
            std::cout<< "has successor(s): ";
            for (int i = 0; i < len; ++i){
                std::cout<< get_module_name((*it).succ_modules[i]) <<", ";
            }
        }
        std::cout<<"\n";
    }
}

const std::map<MODULE_NAME, int> make_module_vertex_map(const Graph_t& graph){
    std::map<MODULE_NAME, int> mvMap;
    for (int i = 0; i < graph.size(); ++i){
        mvMap.insert(std::pair<MODULE_NAME, int>(graph[i].module, i));
    }
    return mvMap;
}

const int find_index_for_module(const MODULE_NAME m, const std::map<MODULE_NAME, int>& mvMap){
    return mvMap.find(m)->second;
}


typedef enum{
    UNVISITED,
    VISITING,
    VISITED
} VISIT_STATUS; // this is only for dfs topological sort

bool dfs_topsort(const MODULE_NAME u, const Graph_t& graph, VISIT_STATUS* vStatus, const std::map<MODULE_NAME, int>& mvMap, MODULE_NAME* sorted, int* ind){
    // topological sort vertices of directed acyclic graph, using DFS. this is not the only way.
    vStatus[find_index_for_module(u, mvMap)] = VISITING;
    AdjVtx_t node = graph[find_index_for_module(u, mvMap)];
    for (int j = 0; j < node.succ_modules.size(); ++j){
        MODULE_NAME v = node.succ_modules[j];
        if (vStatus[find_index_for_module(v, mvMap)] == VISITING){
            // in this case, dfs tries to visit some predecessor. it means cycle exists, cannot topsort.
            return false;
        }
        if (vStatus[find_index_for_module(v, mvMap)] == UNVISITED){
            if ( ! dfs_topsort(v, graph, vStatus, mvMap, sorted, ind)){
                return false; // if the recursion returns false, the current "stack" should also return false.
                // if do not return false, it will go to the next lines and return true eventually.
            }
        }
    }

    // only if no "false", can the program go to here:
    vStatus[find_index_for_module(u, mvMap)] = VISITED;
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

    std::map<MODULE_NAME, int> mvMap = make_module_vertex_map(graph);

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
    graph[2] = {DUMMY1, {DUMMY3}}; // the directed edges are implicitly shown as from DUMMY1 to DUMMY3
    graph[1] = {DUMMY2, {DUMMY3}}; // the directed edges are implicitly shown as from DUMMY2 to DUMMY3
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