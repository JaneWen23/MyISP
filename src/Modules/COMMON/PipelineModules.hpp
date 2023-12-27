#ifndef _PIPELINE_MODULES_H
#define _PIPELINE_MODULES_H

// this file describes the bridge connecting Modules and Pipeline.
// PipelineModules are the actual modules used in pipeline,
// and they are "packages" of other modules in Modules.

#include <functional>
#include <map>
#include <vector>
#include "common.hpp"
#include "../../Modules/ISP_VIN/Vin.hpp"
#include "../../Modules/ISP_COMPRESSION/MyJXS.hpp"
#include "../../Modules/ISP_CCM/ccm.hpp"


typedef struct{
    // defines the structure of pipeline; does not care about data and arguments.
    // minimal definition for people to write in TOML.
    MODULE_ENUM module;
    IMAGE_FMT inFmt;
    IMAGE_FMT outFmt;
    int inBitDepth;
    int outBitDepth;

} PipeUnit_t; // TODO: or change name to pipe unit min info???

typedef struct{
    MODULE_ENUM module;
    std::vector<MODULE_ENUM> succ_modules; // successor modules; a module is a vertex
} AdjVtx_t; // minimal definition for people to write in TOML.

typedef std::vector<AdjVtx_t> Graph_t;

typedef struct{
    MODULE_ENUM module;
    std::vector<MODULE_ENUM> pred_modules; // predecessor modules; a module is a vertex
    std::vector<MODULE_ENUM> succ_modules; // successor modules; a module is a vertex
} PipeNode_t; // "full", and will be inferred from graph

// TODO: move this to .cpp!!!
void print_graph(Graph_t& graph){
    for(auto it = graph.begin(); it != graph.end(); ++it){
        std::cout<< "node "<< get_module_name((*it).module)<<": ";
        int L = ((*it).succ_modules).size();
        if (L == 0){
            std::cout<< "has no successor;";
        }
        else{
            std::cout<< "has successor(s): ";
            for (int i = 0; i < L; ++i){
                std::cout<< get_module_name((*it).succ_modules[i]) <<", ";
            }
        }
        std::cout<<"\n";
    }
}

void generate_full_info_pipe_node(const Graph_t& graph, PipeNode_t& pipeNode){
    
}

// TODO: move this to .cpp!!!
const std::map<MODULE_ENUM, int> make_module_vertex_map(const Graph_t& graph){
    std::map<MODULE_ENUM, int> mvMap;
    for (int i = 0; i < graph.size(); ++i){
        mvMap.insert(std::pair<MODULE_ENUM, int>(graph[i].module, i));
    }
    return mvMap;
}

const int find_index_for_module(const MODULE_ENUM m, const std::map<MODULE_ENUM, int>& mvMap){
    return mvMap.find(m)->first;
}

void dfs_topsort(const MODULE_ENUM u, const Graph_t& graph, bool* isVisited, const std::map<MODULE_ENUM, int>& mvMap, MODULE_ENUM* sorted, int* ind){
    // topological sort vertices of directed acyclic graph, using DFS. this is not the only way.
    AdjVtx_t node = graph[find_index_for_module(u, mvMap)];
    for (int j = 0; j < node.succ_modules.size(); ++j){
        MODULE_ENUM v = node.succ_modules[j];
        if ( ! isVisited[find_index_for_module(v, mvMap)]){
            dfs_topsort(v, graph, isVisited, mvMap, sorted, ind);
        }
    }

    isVisited[u] = true;
    sorted[*ind] = u;
    (*ind)--;
}

// TODO: move this to .cpp!!!
void topological_sort(const Graph_t& graph, MODULE_ENUM* sorted){
    std::map<MODULE_ENUM, int> mvMap = make_module_vertex_map(graph);

    int N = graph.size();
    bool isVisited[N];
    for (int i = 0; i < N; ++i){
        isVisited[i] = false;
    }

    int ind = N-1;
    for (int i = 0; i < N; ++i){
        if(!isVisited[i]){
            MODULE_ENUM u = graph[i].module;
            dfs_topsort(u, graph, isVisited, mvMap, sorted, &ind);
        }
    }
}

// TODO: move this to .cpp!!!
void print_sorted_nodes(const MODULE_ENUM* sorted, const int len){
    std::cout<<"sorted:\n";
    for (int i = 0; i < len - 1; ++i){
        std::cout<< get_module_name(sorted[i])<<" --> ";
    }
    std::cout<< get_module_name(sorted[len-1]);
    std::cout<<"\n";
}


// TODO: move this to .cpp!!!
bool test_topological_sort(){
    int n = 9; // number of nodes

    Graph_t graph(n);

    // "full":
    // graph[0] = {DUMMY0, {}, {DUMMY1, DUMMY2}}; // the directed edges are implicitly shown as from DUMMY0 to DUMMY1, and from DUMMY0 to DUMMY2
    // graph[1] = {DUMMY1, {DUMMY0}, {DUMMY3}}; // the directed edges are implicitly shown as from DUMMY1 to DUMMY3
    // graph[2] = {DUMMY2, {DUMMY0}, {DUMMY3}}; // the directed edges are implicitly shown as from DUMMY2 to DUMMY3
    // graph[3] = {DUMMY3, {DUMMY1, DUMMY2}, {DUMMY4, DUMMY5}}; // and so on ...
    // graph[4] = {DUMMY4, {DUMMY3}, {DUMMY6}};
    // graph[5] = {DUMMY5, {DUMMY3}, {DUMMY7}};
    // graph[6] = {DUMMY6, {DUMMY4, DUMMY7}, {DUMMY8}};
    // graph[7] = {DUMMY7, {DUMMY5}, {DUMMY6}}; 
    // graph[8] = {DUMMY8, {DUMMY6}, {}};

    graph[0] = {DUMMY0, {DUMMY1, DUMMY2}}; // the directed edges are implicitly shown as from DUMMY0 to DUMMY1, and from DUMMY0 to DUMMY2
    graph[1] = {DUMMY1, {DUMMY3}}; // the directed edges are implicitly shown as from DUMMY1 to DUMMY3
    graph[2] = {DUMMY2, {DUMMY3}}; // the directed edges are implicitly shown as from DUMMY2 to DUMMY3
    graph[3] = {DUMMY3, {DUMMY4, DUMMY5}}; // and so on ...
    graph[4] = {DUMMY4, {DUMMY6}};
    graph[5] = {DUMMY5, {DUMMY7}};
    graph[6] = {DUMMY6, {DUMMY8}};
    graph[7] = {DUMMY7, {DUMMY6}}; 
    graph[8] = {DUMMY8, {}};

    //print_graph(graph);

    MODULE_ENUM sorted[n];
    topological_sort(graph, sorted);
    print_sorted_nodes(sorted, n);

    return true;
}

typedef struct{
    // this struct is usually inferred from PipeUnit_t;
    // i.e., full definition.
    MODULE_ENUM module;
    IMAGE_FMT inFmt;
    IMAGE_FMT outFmt;
    int inBitDepth;
    int outBitDepth;

    // TODO: module cfg will be carried out in pipeline class constructor
 
    std::function<IMG_RTN_CODE(const ImgPtrs_t, Img_t*, void*)> run_function;
} Module_t;

// Update 20231218
// forget the above. need to rethink about the modules in pipeline
// it is certain that a pipeline topology is a DAG (Directional Acyclic Graph)
// a module in a DAG is a node, so,
// a module may have uncertain number of input images
// and a module output may be used by multiple modules
// "module's function is different than Algo's function"
// module's interface:
// still generate one output Img_t (shared pointer), but input img is in list
// about the args: "module arg" may contain algo arg and more, for example, some adaptive args between two algos.
// and the args should contain the items to be memorized, like, adaptive n-tap filter coefficients => n numbers should be in args.
// modules should also take care of the adaptation of args during the running of stream,
// so, the module function may contain: 1, execution of this frame, and 2, update of args.
// so, the output should be an outImg and updatedArg (maybe update arg in the same place)

// the first input image should be the "main" image that being active during whole pipeline processing
// the pipeline should contain one main image and some buffered images. 
// only in the context of pipeline, need people to consider where to store the images.
// the inFmt, outFmt, ... : modules do not contain these info, but need to check every input's fmt and output fmt
// some algo are restricted to a certain fmt, so module needs to ensure that fmt is correct.
// modules i/o should be unsigned only. need to convert to unsigned if algo produces signed results.
// this gives rise to the problem of bit-depth conversion, since signed <==> unsigned may be complicated.
// and bit depth conversion may or may not cause the change of data type in cpp.
// however, we need to take care of these two cases.

// the pipeline itself may not determine "pipeline bit depth" independently; the bit depth should be determined by
// combination of sensor bit depth and each module's format and sign(defined by algo).
// pipeline bit depth is just a result of the modules' bit depth.

// what algos can be packed into a module: algos in serial, or just the one algo;
// two parallel algos should not be packed into one module, they should be two modules with proper names.
// modules may not contain the info about next or last module
// pipeline at construction: check in fmt of this module and out fmt of the last module. (what if two input formats?)

// every output has to be clear that which module(s) will be using it.
// and every output image should record the module name who made the output.
// if two modules will be using it, just record the names in list.
// when a new module is to be executed, the output image pointer should be "delivered" to "input pool", output ptr will be reset;
// and then "tells" the input image, now it is module xxx using the input image.
// after module xxx used image, the module name should be removed from list.
// after all modules finished using input images (list is empty), the input image will be freed.
// if an input image will be used latter, but will not be used by the next module:
// since the input image is not freed, just deliver the next module's input to another ptr in input pool.
// if a module needs two or more inputs, since every output knows who made it and there is dependency list, 
// we can get the corresponding inputs from the named-outputs.

Module_t generate_isp_module(PipeUnit_t& sPipeUnit);
void test_pipeline_modules();

#endif