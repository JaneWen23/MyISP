#include "pipeline.hpp"
#include <iostream>
#include "parse.hpp"

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
        bool isDone = false;
        for(auto ip = pipe.begin(); ip != pipe.end(); ++ip){
            if ((*ip).module == (*it).module){ // ip is subset of it, and it is subset of ip
                if (is_subset((*it).inputInOrder, (*ip).predWthDelay) && is_subset((*ip).predWthDelay, (*it).inputInOrder)){
                    (*ip).predWthDelay = (*it).inputInOrder;
                    isDone = true;
                    break;
                }
                else{
                    std::cout<<"error: the provided predecessors of "<< get_module_name((*it).module) << " do not match with the graph. exited.\n";
                    exit(1);
                }
            }
        }
        if ( ! isDone){
            std::cout<<"error: "<< get_module_name((*it).module) <<" from 'orders' is not defined in the pipeline. exited.\n";
            exit(1);
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
    std::unordered_map<MODULE_NAME, int> mvMap = make_module_vertex_map(graphNoDelay);
    const int n = graphNoDelay.size();
    if (n < 1){
        std::cout<<"error: pipe is not created with a proper length. exited.\n";
        exit(1);
    }
    for (int i = 0; i < n; ++i){
        // copy the module name and the successor modules:
        pipe[i].module = sorted[i];
        int ii = mvMap.at(sorted[i]);
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

void make_pipe_with_delay(const Orders_t& orders, const Graph_t& graphNoDelay, const DelayGraph_t& delayGraph, const MODULE_NAME* sorted, Pipe_t& pipe){
    copy_graph_to_pipe(graphNoDelay, sorted, pipe);
    attach_delay_to_pipe(delayGraph, pipe);
    // for modules with two or more inputs:
    // reorder the predecessors to match with the order of "main, additional1, additional2, ..."
    // the order is specified by cfg (user-provided extra information)
    reorder_predecessors(orders, pipe);
}


void print_pipe(const Pipe_t& pipe){
    std::cout<<"pipe:\n";
    int i = 0;
    for(auto it = pipe.begin(); it != pipe.end(); ++it){
        i += 1;
        std::cout << i << ") "<< get_module_name((*it).module)<<": ";
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
    std::cout<<"\n";
}

void Pipeline::set_pipeline(const Graph_t& graphNoDelay, const DelayGraph_t& delayGraph, const Orders_t& orders, bool needPrint){
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
    _hsOneFrame = get_hash_one_frame_from_modules();
    std::cout<< _cfgDoneInfo;
    //print_hash(&_hsOneFrame, "default args:");
}

Pipeline::Pipeline(const Graph_t& graphNoDelay, const DelayGraph_t& delayGraph, const Orders_t& orders, bool needPrint){
    set_pipeline(graphNoDelay, delayGraph, orders, needPrint);
}

void Pipeline::set_pipeline(const Graph_t& graphNoDelay, const Orders_t& orders, bool needPrint){
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
    _hsOneFrame = get_hash_one_frame_from_modules();
    std::cout<< _cfgDoneInfo;
    //print_hash(&_hsOneFrame, "default args:");
}

Pipeline::Pipeline(const Graph_t& graphNoDelay, const Orders_t& orders, bool needPrint){
   set_pipeline(graphNoDelay, orders, needPrint);
}

void Pipeline::set_pipeline(const Graph_t& graphNoDelay, bool needPrint){
    // initialize _sorted and _pipe according to number of nodes:
    int n = graphNoDelay.size();
    _sorted = (MODULE_NAME*)malloc( n * sizeof(MODULE_NAME));
    _pipe = Pipe_t(n);
    // generate pipeline from graph:
    topological_sort(graphNoDelay, _sorted);
    copy_graph_to_pipe(graphNoDelay, _sorted, _pipe);
    if (needPrint){
        print_pipe(_pipe);
    }
    _hsOneFrame = get_hash_one_frame_from_modules();
    std::cout<< _cfgDoneInfo;
    //print_hash(&_hsOneFrame, "default args:");
}

Pipeline::Pipeline(const Graph_t& graphNoDelay, bool needPrint){
    set_pipeline(graphNoDelay, needPrint);
}

Pipeline::Pipeline(const char* file, bool needPrint){
    ParsedPipe parsedPipe(file);
    bool hasGraphNoDelay = parsedPipe.get_info().at("graphNoDelay");
    bool hasDelayGraph = parsedPipe.get_info().at("delayGraph");
    bool hasOrders = parsedPipe.get_info().at("orders");
    if(hasGraphNoDelay && hasDelayGraph && hasOrders){
        set_pipeline(parsedPipe.get_graph_no_delay(), parsedPipe.get_graph_with_delay(), parsedPipe.get_input_orders(), needPrint);
    }
    else if(hasGraphNoDelay && (!hasDelayGraph) && hasOrders){
        set_pipeline(parsedPipe.get_graph_no_delay(), parsedPipe.get_input_orders(), needPrint);
    }
    else if(hasGraphNoDelay && (!hasDelayGraph) && (!hasOrders)){
        set_pipeline(parsedPipe.get_graph_no_delay(), needPrint);
    }
    else{
        std::cout<<"error: the pipe config file is not able to define a pipeline. exited.\n";
        exit(1);
    }
}

Pipeline::~Pipeline(){
    free(_sorted);
    _pipe.clear();
}

Hash_t Pipeline::get_hash_one_frame_from_modules(){
    Hash_t hsOneFrame;
    for(auto it = _pipe.begin(); it!= _pipe.end(); ++it){
        std::string moduleName = get_module_name((*it).module);
        Hash_t tmpHash = get_default_arg_hash_for_module((*it).module);
        hsOneFrame.insert({moduleName, tmpHash});
    }
    return hsOneFrame;
}

void Pipeline::generate_arg_cfg_template(int frameNum){
    if (frameNum < 1){
        std::cout<<"error: should generate config file for at least 1 frame, but got " << frameNum << " instead. exited.\n";
        exit(1);
    }
    Hash_t argHyperHash;
    std::string rootKey0 = _configFrameStr + std::to_string(0);
    argHyperHash.insert({rootKey0, _hsOneFrame});
    Hash_t emptyHs;
    for (int frameInd = 1; frameInd < frameNum; ++frameInd){
        std::string rootKey = _configFrameStr + std::to_string(frameInd);
        argHyperHash.insert({rootKey, emptyHs});
    }
    // convert hash to toml table and dump toml table as base (to be employed in a func in parse.cpp)
    generate_toml_file_from_hash(_baseCfgFilePath.c_str(), &argHyperHash);
    std::cout<< _baseCfgInfo;
}


void Pipeline::move_output_to_pool(){
    // if no delivery (or no output at all), do nothing. // TODO: problem: output is not null but no delivery: not allowed. add logic to return error.
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
            if ((*it).sig.madeBy.timeStamp + sModule.predWthDelay[i].delay == _frameInd){
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
    // if the list is empty, destroy the img.
    int len = sModule.predWthDelay.size();
    for (int i = 0; i < len; ++i){
        for (auto it = _InImgPool.begin(); it != _InImgPool.end(); ++it){
            if ((*it).sig.madeBy.timeStamp + sModule.predWthDelay[i].delay == _frameInd){
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
    // assemble the img and signature (img was set by module 'run_function')
    _sOutPipeImg.sig.madeBy.module = sModule.module;
    _sOutPipeImg.sig.madeBy.timeStamp = _frameInd;
    for (auto it = sModule.succWthDelay.begin(); it != sModule.succWthDelay.end(); ++it){
        _sOutPipeImg.sig.deliverTo.push_back((*it));
    }
}

void Pipeline::run_module(const Module_t& sModule, Hash_t* pHs, bool updateArgs){
    std::cout<<"running module " << get_module_name(sModule.module) <<":\n";
    move_output_to_pool();
    ImgPtrs_t inPtrs = distribute_in_img_t(sModule);
    sModule.run_function(inPtrs, &(_sOutPipeImg.img), pHs, updateArgs);
    sign_out_from_pool(sModule);
    signature_output_img(sModule);
    std::cout<< get_module_name(sModule.module) <<" Done.\n\n";
}

void Pipeline::clear_imgs(){
    // this should be used only after all frames are processed.
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

void Pipeline::run_pipe(Hash_t* pHsOneFrame, bool updateArgs){
    if (!_pipe.empty()){
        Pipe_t::iterator it;
        for (it = _pipe.begin(); it != _pipe.end(); ++it){
            run_module((*it), find_arg_hash_for_module(pHsOneFrame, (*it).module), updateArgs);

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




    // frame-delayed output:
    // should also be pipeImg type. because output form Pipeline may also be used after delay.

    // from node n0 to itself or node topsort-ed before n0: must delay >= 1 frame
    // from node n0 to any other node in the rest: can have no delay

    // img can be deleted only when delivery list is empty

    // for top-sort, only need no-delay nodes
    // after that, need delay info at all times
    // no-delay graph is a special case.
    // and after top-sort, we discuss general case.

void Pipeline::default_run_pipe(int frameNum){
    // use default args (hash) to run frames.
    // note: module with delay needs to ensure that initial frames are well-behaved, however, it is the module's responsibility
    if (frameNum < 1){
        std::cout<<"error: pipe should run for at least 1 frame, but got " << frameNum << " instead. exited.\n";
        exit(1);
    }
    Hash_t hsOneFrame = get_hash_one_frame_from_modules();
    for (_frameInd = 0; _frameInd < frameNum; ++_frameInd){
        std::cout<< "\n======== [default mode] running frame #"<< _frameInd <<": ========\n\n";
        run_pipe(&hsOneFrame, true);
    }
    std::cout<<"\n======== [default mode] all frames processed. ========\n\n";
    clear_imgs();
}

void Pipeline::cfg_run_pipe(const char* filePath, int frameNum){
    if (frameNum < 1){
        std::cout<<"error: pipe should run for at least 1 frame, but got " << frameNum << " instead. exited.\n";
        exit(1);
    }
    if (strcmp(filePath, _baseCfgFilePath.c_str()) == 0){
        std::cout<<"\nwarning: config file name is the same as default. "
        "if you really mean to use default arguments, consider call default_run_pipe() instead; "
        "otherwise, use another name for your config file.\n\n";
    }
        // when update by algo and when a frame is finished, dump the updated arg hash in toml (generate toml file from hash);
        // when update by toml, do not need to dump arg (or you can switch on/off)

        // cases are:
        // 1, you have a hand-made init arg in toml, want to update by algo
        // 2, you have a hand-made init arg in hash, want to update by algo (not recommended, if hand-made, then use toml)
        // 3, you do not have any init arg and would like to use default arg, and update by algo
        // 4, you have a series of frames arg in toml, want to update by toml (use toml args every frame)

        // ===> case 2 is opt out
        // ===> if no toml provided, program should dump the base toml according to the default args
        // and use default args (in hash) as default input

        // when update by algo, you want to update the hash, so that it's easier to track any 
        // changes (due to the specific way to set value).


        // what to do now:
        // 1) turn cfg file into hash (function realized in parse.cpp; toml tbl is not allowed to appear here)
        // the hash should be stored as something like _hsOneFrame and will be updated (partially) by the next frame.
        // what should be the hash looked like?
        // want frame #0 to be in full args, frame #1 and more to be in 'difference args' only.
        // and then override certain args in an existed hash.
        // however, it is ALLOWED that all args are in frame #1 and more, even if they are not all updated.
        // 2) pass the hash into run_pipe().
    ParsedArgs parsedArgs(filePath);
    int cfgSize = parsedArgs.size();
    if (cfgSize < frameNum){
        std::cout<< "\n\ninfo from cfg_run_pipe():\nthe config file defines args for "<< cfgSize <<" frame(s),";
        std::cout<< " but pipe is asked to run "<< frameNum <<" frames;\n";
        std::cout<< "so, pipe will use automatically updated args after "<< cfgSize <<" frame(s).\n";
    }
    Hash_t hsOneFrame = get_hash_one_frame_from_modules();
    // run frames where args are read from cfg:
    for(_frameInd = 0; _frameInd <  cfgSize; ++_frameInd){
        std::cout<<"\n======== running frame #"<< _frameInd <<": ========\n\n";
        std::string rootKey = _configFrameStr + std::to_string(_frameInd);
        parsedArgs.fetch_args_at_frame(rootKey, &hsOneFrame);
        if (cfgSize >= frameNum){
            // in this case, just read arg from cfg, never update args:
            run_pipe(&hsOneFrame, false);
        }
        else{ // i.e. cfgSize < frameNum
            if(_frameInd < cfgSize - 1){
                // before the last frame defined in cfg, we just should read args from cfg and do not update:
                run_pipe(&hsOneFrame, false);
            }
            else{ // i.e. _frameInd == cfgSize - 1
                // at the last frame defined in cfg, we need to update args for the next frame to use:
                run_pipe(&hsOneFrame, true);
            }
        }
    }
    // run frames where args are updated automatically:
    for(_frameInd = cfgSize; _frameInd < frameNum; ++_frameInd){
        std::cout<<"\n======== running frame #"<< _frameInd <<": ========\n\n";
        std::string rootKey = _configFrameStr + std::to_string(_frameInd);
        run_pipe(&hsOneFrame, true); 
    }
    std::cout<<"\n======== all frames processed. ========\n\n";
    clear_imgs();
}


void test_pipeline(){

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
    //myPipe.default_run_pipe(3);
    // myPipe.cfg_run_pipe("../args/dummyCfg.toml", 2);

}

void test_pipeline2(){

    int n = 2; // number of nodes

    Graph_t graph(n);
    
    graph[0] = {ISP_VIN, {ISP_COMPRESSION}};
    graph[1] = {ISP_COMPRESSION, { }};


    Pipeline myPipe(graph, true);
    //myPipe.generate_arg_cfg_template(0,4);
    //myPipe.cfg_run_pipe("../args/base2_want.toml", 2);

    //myPipe.default_run_pipe(2);

}

void test_pipeline3(){
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

    Orders_t orders;
    orders.push_back({DUMMY3, {{DUMMY1}, {DUMMY2}}}); // mind the syntax! {DUMMY1} is actually {DUMMY1, 0}, the 0 is default and therefore omitted.
    orders.push_back({DUMMY6, {{DUMMY4}, {DUMMY5}, {DUMMY7}}});

    Pipeline myPipe(graph, orders, true);
    //myPipe.generate_arg_cfg_template(0,4);
    //myPipe.cfg_run_pipe("../args/base2_want.toml", 2);

    //myPipe.default_run_pipe(2);

}

void test_pipeline4(){

    int n = 4; // number of nodes

    Graph_t graph(n);
    
    graph[0] = {DUMMY0, {DUMMY1, DUMMY2}}; // the directed edges are implicitly shown as from DUMMY0 to DUMMY1, and from DUMMY0 to DUMMY2
    graph[2] = {DUMMY1, {DUMMY3}}; // the directed edges are implicitly shown as from DUMMY1 to DUMMY3
    graph[1] = {DUMMY2, {DUMMY3}}; // the directed edges are implicitly shown as from DUMMY2 to DUMMY3
    graph[3] = {DUMMY3, { }};
    

    DelayGraph_t delayGraph;
    delayGraph.push_back({DUMMY3, {{DUMMY3, 1}}});
    delayGraph.push_back({DUMMY1, {{DUMMY2, 1}}});

    Orders_t orders;
    orders.push_back({DUMMY3, {{DUMMY1 }, {DUMMY2 }, {DUMMY3, 1}}}); // mind the syntax! {DUMMY1} is actually {DUMMY1, 0}, the 0 is default and therefore omitted.
    orders.push_back({DUMMY2, {{DUMMY0 }, {DUMMY1, 1}}});


    Pipeline myPipe(graph, delayGraph, orders, true);
    //Pipeline myPipe(graph, orders, true);
    //myPipe.default_run_pipe(3);
    myPipe.cfg_run_pipe("../args/dummyCfg.toml", 3);

}

void test_pipeline_config(){
    // ParsedPipe myParsedPipe("../pipeCfg/pipeCfgDummy2.toml");
    // Graph_t graph = myParsedPipe.get_graph_no_delay();
    // DelayGraph_t delayGraph = myParsedPipe.get_graph_with_delay();
    // Orders_t orders = myParsedPipe.get_input_orders();

    //Pipeline myPipe(graph, delayGraph, orders, true);

    Pipeline myPipe2("../pipeCfg/pipeCfgDummy3.toml", true);
    //Pipeline myPipe2("../pipeCfg/pipeCfgDummy1.toml", true);
    //Pipeline myPipe2("../pipeCfg/pipeCfgDummy2.toml", true);
}