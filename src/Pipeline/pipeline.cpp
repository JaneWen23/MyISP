#include "pipeline.hpp"
#include <algorithm>
#include <iostream>
#include "../ThirdParty/OpenCV.hpp"

bool is_ModuleDelay_equal(const ModuleDelay_t& a, const ModuleDelay_t& b){
    if (a.module == b.module && a.delay == b.delay){
        return true;
    }
    return false;
}

bool is_subset(const std::vector<ModuleDelay_t>& a, const std::vector<ModuleDelay_t>& b){
    // check if a is subset of b.
    // this is naive way, for convenience.
    bool ans = false;
    for (auto ia = a.begin(); ia != a.end(); ++ia){
        for (auto ib = b.begin(); ib != b.end(); ++ib){
            if (is_ModuleDelay_equal(*ia, *ib)){
                ans = true;
                break;
            }
            ans = false;
        }
        if (!ans) break;
    }
    return ans;
}

bool is_subset(const ModuleDelay_t& a, const std::vector<ModuleDelay_t>& b){
    // check if a is subset of b.
    // this is naive way, for convenience.
    for (auto ib = b.begin(); ib != b.end(); ++ib){
        if (is_ModuleDelay_equal(a, *ib)){
            return true;
        }
    }
    return false;
}

void reorder_predecessors(const Orders_t& orders, Pipe_t& pipe){
    // first check if predecessors is subset of cfg-provided,
    // then check if cfg-provided is subset of predecessors,
    // if both are true, replace predecessors with cfg-provided,
    // if not, report an error and exit.
    for (auto it = orders.begin(); it != orders.end(); ++it){
        for(auto ip = pipe.begin(); ip != pipe.end(); ++ip){
            if ((*ip).module == (*it).module){
                if (is_subset((*it).inputInOrder, (*ip).predWthDelay) && is_subset((*ip).predWthDelay, (*it).inputInOrder)){
                    (*ip).predWthDelay = (*it).inputInOrder;
                }
                else{
                    std::cout<<"error: the provided predecessors of "<< get_module_name((*it).module) << " do not match with the graph. exited.\n";
                    exit(1);
                }
                break;
            }
        }
    }
}

// delayGraph: the graph describes the delayed vertices and edges. delay must >= 1.
// Pipe_t: i.e. Full Graph, graph with delay
// generate graph with delay from a series of graphs
// should do top sort first
// then copy to Pipe_t with delay=0
// then add delayed succ and pred according to delayGraph


void copy_no_delay_to_delayed(const std::vector<MODULE_NAME>& noDelay, std::vector<ModuleDelay_t>& delayed){
    for (auto it = noDelay.begin(); it != noDelay.end(); ++it){
        delayed.push_back({*it, 0});
    }
}

void copy_graph_to_pipe(const Graph_t& graphNoDelay, const MODULE_NAME* sorted, Pipe_t& pipe){
    std::map<MODULE_NAME, int> mvMap = make_module_vertex_map(graphNoDelay);
    const int n = graphNoDelay.size();
    if (n < 1){
        std::cout<<"error: pipe is not created with a proper length. exited.\n";
        exit(1);
    }
    for (int i = 0; i < n; ++i){
        // copy the module name and the successor modules:
        pipe[i].module = sorted[i];
        int ii = find_index_for_module(sorted[i], mvMap);
        copy_no_delay_to_delayed(graphNoDelay[ii].succModules, pipe[i].succWthDelay);
        // then "insert" predecessor modules:
        for (int jj = 0; jj < graphNoDelay[ii].succModules.size(); ++jj){
            int j = find_ind_in_sorted(graphNoDelay[ii].succModules[jj], sorted, n); 
            pipe[j].predWthDelay.push_back({graphNoDelay[ii].module, 0});
        }
        // find module-run-function:
        pipe[i].run_function = find_func_for_module(sorted[i]);
    }
}

void attach_delay_to_pipe(const DelayGraph_t& delayGraph, Pipe_t& pipe){
    // attach delayed successors:
    for (auto it = delayGraph.begin(); it != delayGraph.end(); ++it){
        for (auto ip = pipe.begin(); ip != pipe.end(); ++ip){
            if ((*it).module == (*ip).module){
                for(auto ii = (*it).succWthDelay.begin(); ii != (*it).succWthDelay.end(); ++ii){
                    if ((*ii).delay > 0){
                        (*ip).succWthDelay.push_back(*ii);
                    }
                    else{
                        std::cout<<"error: invalid delay. the delay in delayGraph must >= 1. exited.\n";
                        exit(1);
                    }
                }
                break;
            }
        }
    }
    // attach delayed predecessors:
    for (auto it = delayGraph.begin(); it != delayGraph.end(); ++it){
        for(auto ii = (*it).succWthDelay.begin(); ii != (*it).succWthDelay.end(); ++ii){
            for (auto ip = pipe.begin(); ip != pipe.end(); ++ip){
                if ((*ip).module == (*ii).module){
                    (*ip).predWthDelay.push_back({(*it).module, (*ii).delay});
                    break;
                }
            }
        }
    }
}

void make_pipe_no_delay(const Orders_t& orders, const Graph_t& graphNoDelay, const MODULE_NAME* sorted, Pipe_t& pipe){
    copy_graph_to_pipe(graphNoDelay, sorted, pipe);
    reorder_predecessors(orders, pipe); // NOTE: if no-delay only, you should also provide "no-delay orders"
}

void make_pipe_with_delay(const Orders_t& orders, const Graph_t& graphNoDelay, const DelayGraph_t delayGraph, const MODULE_NAME* sorted, Pipe_t& pipe){
    copy_graph_to_pipe(graphNoDelay, sorted, pipe);
    attach_delay_to_pipe(delayGraph, pipe);
    // for modules with two or more inputs:
    // reorder the predecessors to match with the order of "main, additional1, additional2, ..."
    // the order is specified by cfg (user-provided extra information)
    reorder_predecessors(orders, pipe);
}


void print_pipe(const Pipe_t& pipe){
    std::cout<<"pipe:\n";
    for(auto it = pipe.begin(); it != pipe.end(); ++it){
        std::cout << get_module_name((*it).module)<<": ";
        int lp = ((*it).predWthDelay).size();
        if (lp == 0){
            std::cout<< "  needs no input; ";
        }
        else{
            std::cout<< "  takes input(s) from: ";
            for (int i = 0; i < lp; ++i){
                std::cout<< get_module_name((*it).predWthDelay[i].module);
                if ((*it).predWthDelay[i].delay == 0){
                    std::cout<<", ";
                }
                else if ((*it).predWthDelay[i].delay == 1) {
                    std::cout<<"(last frame), ";
                }
                else if ((*it).predWthDelay[i].delay == 2){
                    std::cout<<"(last 2nd frame), ";
                }
                else if ((*it).predWthDelay[i].delay == 3){
                    std::cout<<"(last 3rd frame), ";
                }
                else{
                    std::cout<<"(last "<< (*it).predWthDelay[i].delay <<"th frame), ";
                }
            }
        }

        int ls= ((*it).succWthDelay).size();
        if (ls == 0){
            std::cout<< "  dose not deliver output; ";
        }
        else{
            std::cout<< "  delivers output to: ";
            for (int i = 0; i < ls; ++i){
                std::cout<< get_module_name((*it).succWthDelay[i].module);
                if ((*it).succWthDelay[i].delay == 0){
                    std::cout<<", ";
                }
                else if ((*it).succWthDelay[i].delay == 1) {
                    std::cout<<"(next frame), ";
                }
                else if ((*it).succWthDelay[i].delay == 2){
                    std::cout<<"(next 2nd frame), ";
                }
                else if ((*it).succWthDelay[i].delay == 3){
                    std::cout<<"(next 3rd frame), ";
                }
                else{
                    std::cout<<"(next "<< (*it).succWthDelay[i].delay <<"th frame), ";
                }
            }
        }
        std::cout<<"\n";
    }
}


Pipeline::Pipeline(const Graph_t& graphNoDelay, const DelayGraph_t delayGraph, const Orders_t& orders, bool needPrint){
    // initialize _sorted and _pipe according to number of nodes:
    int n = graphNoDelay.size();
    _sorted = (MODULE_NAME*)malloc( n * sizeof(MODULE_NAME));
    _pipe = Pipe_t(n);
    // generate pipeline from graph:
    topological_sort(graphNoDelay, _sorted);
    make_pipe_with_delay(orders, graphNoDelay, delayGraph, _sorted, _pipe);
    if (needPrint){
        print_pipe(_pipe);
    }
}

Pipeline::Pipeline(const Graph_t& graphNoDelay, const Orders_t& orders, bool needPrint){
    // initialize _sorted and _pipe according to number of nodes:
    int n = graphNoDelay.size();
    _sorted = (MODULE_NAME*)malloc( n * sizeof(MODULE_NAME));
    _pipe = Pipe_t(n);
    // generate pipeline from graph:
    topological_sort(graphNoDelay, _sorted);
    make_pipe_no_delay(orders, graphNoDelay, _sorted, _pipe);
    if (needPrint){
        print_pipe(_pipe);
    }
}

Pipeline::~Pipeline(){
    free(_sorted);
    _pipe.clear();
}

void Pipeline::move_output_to_pool(){
    // if no delivery (or no output at all), do nothing.
    if ( ! _sOutPipeImg.sig.deliverTo.empty()){
        _InImgPool.push_back(_sOutPipeImg);
        for (int i = 0; i < MAX_NUM_P; ++i){
            _sOutPipeImg.img.pImageData[i] = NULL;
        }
        _sOutPipeImg.sig.deliverTo.clear();
    }
}

const ImgPtrs_t Pipeline::distribute_in_img_t(const Module_t& sModule){
    // this function should be called after the output img moved to input pool.
    // assign the input images from pool to the module run-function.
    int len = sModule.predWthDelay.size();
    ImgPtrs_t imgPtrs(len);
    for (int i = 0; i < len; ++i){
        imgPtrs[i] = NULL;
    }
    for (int i = 0; i < len; ++i){
        for (auto it = _InImgPool.begin(); it != _InImgPool.end(); ++it){
            if ((*it).sig.madeBy.time + sModule.predWthDelay[i].delay == _frameInd){
                if ((*it).sig.madeBy.module == sModule.predWthDelay[i].module){
                    if (is_subset({sModule.module, sModule.predWthDelay[i].delay}, (*it).sig.deliverTo)){
                        imgPtrs[i] = &((*it).img); // assign img
                        break;
                    }   
                }
            }
        }
    }
    return imgPtrs;
}

void remove_from_delivery_list(const ModuleDelay_t m, std::vector<ModuleDelay_t>& ls){
    for (int i = 0; i < ls.size(); ++i){
        if (is_ModuleDelay_equal(m, ls[i])){
            ls.erase(ls.begin()+i);
            return;
        }
    }
}

void Pipeline::sign_out_from_pool(const Module_t& sModule){
    // after input img is used, remove module name from deliverTo list;
    // if delay==0 AND list is empty, destroy the img.
    int len = sModule.predWthDelay.size();
    for (int i = 0; i < len; ++i){
        for (auto it = _InImgPool.begin(); it != _InImgPool.end(); ++it){
            if ((*it).sig.madeBy.time + sModule.predWthDelay[i].delay == _frameInd){
                if ((*it).sig.madeBy.module == sModule.predWthDelay[i].module){
                    if (is_subset({sModule.module, sModule.predWthDelay[i].delay}, (*it).sig.deliverTo)){
                        remove_from_delivery_list({sModule.module, sModule.predWthDelay[i].delay}, (*it).sig.deliverTo);

                        if ((*it).sig.deliverTo.size() == 0){
                            free_image_data(&(*it).img);
                            _InImgPool.erase(it);
                        }
                        break;
                    }   
                }
            }
        }
    }
}

void Pipeline::signature_output_img(const Module_t& sModule){
    // assemble the img and signature (img was set by module run_function)
    _sOutPipeImg.sig.madeBy.module = sModule.module;
    _sOutPipeImg.sig.madeBy.time = _frameInd;
    for (auto it = sModule.succWthDelay.begin(); it != sModule.succWthDelay.end(); ++it){
        _sOutPipeImg.sig.deliverTo.push_back((*it));
    }
}

void Pipeline::run_module(const Module_t& sModule, void* pMArg){
    std::cout<<"running module " << get_module_name(sModule.module) <<":\n";
    move_output_to_pool();
    ImgPtrs_t inPtrs = distribute_in_img_t(sModule);
    sModule.run_function(inPtrs, &(_sOutPipeImg.img), pMArg);
    sign_out_from_pool(sModule);
    signature_output_img(sModule);
}

void Pipeline::clear_imgs(){
    // this should be used after all frames are processed.
    if (_InImgPool.size() > 0){
        for (auto it = _InImgPool.begin(); it != _InImgPool.end(); ++it){
            free_image_data(&(*it).img);
        }
        _InImgPool.clear();
    }
    if ( ! _sOutPipeImg.sig.deliverTo.empty()){
        free_image_data(&(_sOutPipeImg.img));
    }
}

void Pipeline::run_pipe(AllArgs_t& sArgs){
    if (!_pipe.empty()){
        Pipe_t::iterator it;
        for (it = _pipe.begin(); it != _pipe.end(); ++it){
            run_module((*it), find_arg_for_func(sArgs, (*it).module));

            // TODO:  maybe dump?? "EOF end of frame"
            //dump();
        }
        // maybe dump some log here??
        if (_sOutPipeImg.sig.deliverTo.empty()){
            free_image_data(&(_sOutPipeImg.img));
        }
    }
    else{
        std::cout<<"warning: pipeline is empty. nothing processed.\n";
    }
}

// void Pipeline::dump(){
//     cv::Mat image;
//     convert_img_t_to_cv_mat(image, &_sOutImg);
//     imwrite("../dump/xxx.png", image); // TODO: file name?? + prefix, protected; RawStreamPipeline should have its own prefix and inherit Pipeline's prefix
// }



void parse_args(const int frameInd, AllArgs_t& sArgs){
    // TODO: two ways to update args. one is c-model simulation mode, just set values when module runs;
    // the other is to read from a series of toml files for every frame, and override the values set by module.

}

    // frame-delayed output:
    // should also be pipeImg type. because output form Pipeline may also be used after delay.

    // from node n0 to itself or node topsort-ed before n0: must delay >= 1 frame
    // from node n0 to any other node in the rest: can have no delay

    // img can be deleted only when both delivery lists are empty

    // want the pool to be shared by both classes, because it's more convenience to push output to pool (because image data only malloced once)

    // for top-sort, only need no-delay nodes
    // after that, need delay info at all times
    // no-delay graph is a special case.
    // and after top-sort, we discuss general case.


void Pipeline::frames_run_pipe(AllArgs_t& sArgs, int startFrameInd, int frameNum){
    for(_frameInd = startFrameInd; _frameInd < startFrameInd + frameNum; ++_frameInd){
        std::cout<<"\n======== running frame #"<< _frameInd <<": ========\n\n";
        parse_args(_frameInd, sArgs); // already updated some args in Modules when pipe runs. this line is to override.
        run_pipe(sArgs); 
    }
    std::cout<<"\n======== all frames processed. ========\n\n";
    clear_imgs();
}

void test_pipeline(){
    ReadRawArg_t sAlgoVinArg = {
        "../data/rawData.raw", //const char* path;
        0, //int frameInd; // read i-th frame, i >= 0, WILL BE UPDATED IN THE RUN-TIME; if rewind = true, this will not be updated
        RAW_RGGB, //IMAGE_FMT imageFormat;
        4256, //int width;
        2848, //int height;
        16, //int bitDepth;
        1, //int alignment;
    };
    MArg_Vin_t sVinArg = {sAlgoVinArg, false};

    StarTetrixArg_t sStarTetrixArg = {
        1,
        2
    };
    StarTetrixArg_t* pStarTetrixArg = &sStarTetrixArg;

    DWTArg_t sDWTArg = {
        0, // int inImgPanelId; // apply dwt to the whole 2D image
        0, // int outImgPanelId;
        TWO_DIMENSIONAL, // ORIENT orient;
        1, // int level;
        LE_GALL_53, // WAVELET_NAME wavelet;
        MIRROR // PADDING padding;
    };
    
    MArg_Compression_t sCompressionArg = {sStarTetrixArg, sDWTArg};

    CCMArg_t sAlgoCCMArg = {
        {278, -10, -8},
        {-12, 269, -8},
        {-10, -3, 272},
    };
    MArg_CCM_t sCCMArg = {sAlgoCCMArg};

    MArg_Dummy_t sDummyArg = {3};

    AllArgs_t sArgs = {sVinArg, sCompressionArg, sCCMArg, sDummyArg};
    //======================================================

    int n = 9; // number of nodes

    Graph_t graph(n);
    
    graph[0] = {DUMMY0, {DUMMY1, DUMMY2}}; // the directed edges are implicitly shown as from DUMMY0 to DUMMY1, and from DUMMY0 to DUMMY2
    graph[2] = {DUMMY1, {DUMMY3}}; // the directed edges are implicitly shown as from DUMMY1 to DUMMY3
    graph[1] = {DUMMY2, {DUMMY3}}; // the directed edges are implicitly shown as from DUMMY2 to DUMMY3
    graph[3] = {DUMMY3, {DUMMY4, DUMMY5}}; // and so on ...
    graph[4] = {DUMMY4, {}};
    graph[5] = {DUMMY5, {DUMMY6}};
    graph[6] = {DUMMY6, {DUMMY8}};
    graph[7] = {DUMMY7, {DUMMY6}}; 
    graph[8] = {DUMMY8, {}};

    DelayGraph_t delayGraph;
    delayGraph.push_back({DUMMY8, {{DUMMY8, 1}}});
    delayGraph.push_back({DUMMY1, {{DUMMY2, 1}, {DUMMY2, 2}}});

    Orders_t orders;
    orders.push_back({DUMMY3, {{DUMMY1 }, {DUMMY2 }}}); // mind the syntax! {DUMMY1} is actually {DUMMY1, 0}, the 0 is default and therefore omitted.
    orders.push_back({DUMMY6, {{DUMMY4 }, {DUMMY5 }, {DUMMY7 }}});
    orders.push_back({DUMMY8, {{DUMMY6 }, {DUMMY8, 1}}});
    orders.push_back({DUMMY2, {{DUMMY0 }, {DUMMY1, 1}, {DUMMY1, 2}}});


    Pipeline myPipe(graph, delayGraph, orders, true);
    //Pipeline myPipe(graph, orders, true);
    //myPipe.run_pipe(sArgs);
    myPipe.frames_run_pipe(sArgs, 0, 3);

}