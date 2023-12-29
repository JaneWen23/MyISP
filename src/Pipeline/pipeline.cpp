#include "pipeline.hpp"
#include <algorithm>
#include <iostream>

void reorder_predecessors(const Orders_t& orders, Pipe_t& pipe){
    // first check if predecessors is subset of cfg-provided,
    // then check if cfg-provided is subset of predecessors,
    // if both are true, replace predecessors with cfg-provided,
    // if not, report an error and exit.
    for (auto it = orders.begin(); it != orders.end(); ++it){
        for(auto ip = pipe.begin(); ip != pipe.end(); ++ip){
            if ((*ip).module == (*it).module){
                if (is_subset((*it).predInOrder, (*ip).pred_modules) && is_subset((*ip).pred_modules, (*it).predInOrder)){
                    (*ip).pred_modules = (*it).predInOrder;
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

void make_pipe(const Orders_t& orders, const Graph_t& graph, const MODULE_NAME* sorted, Pipe_t& pipe){
    std::map<MODULE_NAME, int> mvMap = make_module_vertex_map(graph);
    const int n = graph.size();
    if (n < 1){
        std::cout<<"error: pipe is not created with a proper length. exited.\n";
        exit(1);
    }
    for (int i = 0; i < n; ++i){
        // copy the module name and the successor modules:
        pipe[i].module = sorted[i];
        int ii = find_index_for_module(sorted[i], mvMap);
        pipe[i].succ_modules = graph[ii].succ_modules;
        // then "insert" predecessor modules:
        for (int jj = 0; jj < graph[ii].succ_modules.size(); ++jj){
            int j = find_ind_in_sorted(graph[ii].succ_modules[jj], sorted, n); 
            pipe[j].pred_modules.push_back(graph[ii].module);
        }
        // find module-run-function:
        pipe[i].run_function = find_func_for_module(sorted[i]);
    }
    // for modules with two or more inputs:
    // reorder the predecessors to match with the order of "main, additional1, additional2, ..."
    // the order is specified by cfg (user-provided extra information)
    reorder_predecessors(orders, pipe);
}


void print_pipe(const Pipe_t& pipe){
    std::cout<<"pipe:\n";
    for(auto it = pipe.begin(); it != pipe.end(); ++it){
        std::cout << get_module_name((*it).module)<<": ";
        int lp = ((*it).pred_modules).size();
        if (lp == 0){
            std::cout<< "  has no input; ";
        }
        else{
            std::cout<< "  takes input(s) from: ";
            for (int i = 0; i < lp; ++i){
                std::cout<< get_module_name((*it).pred_modules[i]) <<", ";
            }
        }

        int ls= ((*it).succ_modules).size();
        if (ls == 0){
            std::cout<< "  has no output; ";
        }
        else{
            std::cout<< "  delivers output to: ";
            for (int i = 0; i < ls; ++i){
                std::cout<< get_module_name((*it).succ_modules[i]) <<", ";
            }
        }
        std::cout<<"\n";
    }
}

void* find_arg_for_func(const AllArgs_t& sArgs, const MODULE_NAME m){
    switch (m){
        case DUMMY0:
        case DUMMY1:
        case DUMMY2:
        case DUMMY3:
        case DUMMY4:
        case DUMMY5:
        case DUMMY6:
        case DUMMY7:
        case DUMMY8:{
            return (void*)(&(sArgs.sDummyArg));
        }
        case ISP_VIN:{
            return (void*)(&(sArgs.sVinArg));
        }
        case ISP_COMPRESSION:{
            return (void*)(&(sArgs.sCompressionArg));
        }
        case ISP_BLC:{
            break;
        }
        case ISP_DMS:{
            break;
        }
        case ISP_WB:{
            break;
        }
        case ISP_CCM:{
            return (void*)(&(sArgs.sCCMArg));
        }
        case ISP_RGB2YUV:{
            break;
        }
        default:{
            std::cout<<"error: cannot find arguments for this function to run. exited.\n";
            exit(1);
        }
    }
    return NULL; // this is nonsense. just because it needs a return value.
}

Pipeline::Pipeline(const Graph_t& graph, const Orders_t& orders, bool needPrint){
    // initialize _sorted and _pipe according to number of nodes:
    int n = graph.size();
    _sorted = (MODULE_NAME*)malloc( n * sizeof(MODULE_NAME));
    _pipe = Pipe_t(n); // TODO: is it constructor of std::vector<T> ??? why TYPE(n) ????
    // generate pipeline from graph:
    topological_sort(graph, _sorted);
    make_pipe(orders, graph, _sorted, _pipe);
    if (needPrint){
        print_pipe(_pipe);
    }
    // _pOutPipeImg = (PipeImg_t*)malloc(sizeof(PipeImg_t));
    // _pOutPipeImg->sig.deliverTo = std::vector<MODULE_NAME>(0);
}

Pipeline::~Pipeline(){
    free(_sorted);
    // do not destruct _pipe manually; it was constructed in stack area.
    //free(_pOutPipeImg);
}

// bool Pipeline::is_pipe_valid_till_now(Module_t& sModule){
//     // if _pipe is empty, nothing will go wrong
//     if (!_pipe.empty()){
//         if(_pipe.back().outBitDepth != sModule.inBitDepth){return false;}
//         if(_pipe.back().outFmt != sModule.inFmt){return false;}
//         if(_pipe.back().outSign != sModule.inSign){return false;}
//     }
//     return true;
// }

void Pipeline::move_output_to_pool(){
    // if no delivery (or no output at all), do nothing.
    if ( ! _pOutPipeImg.sig.deliverTo.empty()){
        _InImgPool.push_back(_pOutPipeImg);
        free_image_data(&(_pOutPipeImg.img));
        _pOutPipeImg.sig.deliverTo.clear();
        // _pOutPipeImg = (PipeImg_t*)malloc(sizeof(PipeImg_t));
        // _pOutPipeImg->sig.deliverTo = std::vector<MODULE_NAME>(0);
    }
}

const ImgPtrs_t Pipeline::distribute_in_img_t(const Module_t& sModule){
    // this function should be called after the output img moved to input pool.
    // assign the input images from pool to the module run-function.
    int len = sModule.pred_modules.size();
    ImgPtrs_t imgPtrs(len);
    for (int i = 0; i < len; ++i){
        for (auto it = _InImgPool.begin(); it != _InImgPool.end(); ++it){
            if ((*it).sig.madeBy == sModule.pred_modules[i]){
                if (is_subset(sModule.module, (*it).sig.deliverTo)){
                    imgPtrs[i] = &((*it).img); // assign img
                    break;
                }   
            }
        }
    }
    return imgPtrs;
}

void remove_from_delivery_list(const MODULE_NAME m, std::vector<MODULE_NAME>& ls){
    for (int i = 0; i < ls.size(); ++i){
        if (m == ls[i]){
            ls.erase(ls.begin()+i);
            return;
        }
    }
}

void Pipeline::sign_out_from_pool(const Module_t& sModule){
    // after input img is used, remove module name from deliverTo list;
    // if list is empty, destroy the img.
    int len = sModule.pred_modules.size();
    ImgPtrs_t imgPtrs(len);
    for (int i = 0; i < len; ++i){
        for (auto it = _InImgPool.begin(); it != _InImgPool.end(); ++it){
            if ((*it).sig.madeBy == sModule.pred_modules[i]){
                if (is_subset(sModule.module, (*it).sig.deliverTo)){
                    remove_from_delivery_list(sModule.module, (*it).sig.deliverTo);
                    if ((*it).sig.deliverTo.size() == 0){
                        // free_image_data(&((*it)->img));
                        // free(*it); // want to free the "PipeImg_t", which was malloced when _pOutPipeImg created.
                        _InImgPool.erase(it);
                    }
                    break;
                }   
            }
        }
    }
}

void Pipeline::signature_output_img(const Module_t& sModule){
    // assemble the img and signature (img was set by module run_function)
    _pOutPipeImg.sig.madeBy = sModule.module;
    for (auto it = sModule.succ_modules.begin(); it != sModule.succ_modules.end(); ++it){
        _pOutPipeImg.sig.deliverTo.push_back((*it));
    }
}

void Pipeline::run_pipe(AllArgs_t& sArgs){
    if (!_pipe.empty()){
        Pipe_t::iterator it;
        for (it = _pipe.begin(); it != _pipe.end(); ++it){
            move_output_to_pool();
            ImgPtrs_t inPtrs = distribute_in_img_t(*it);
            (*it).run_function(inPtrs, &(_pOutPipeImg.img), find_arg_for_func(sArgs, (*it).module));
            sign_out_from_pool(*it);
            signature_output_img(*it);
            // TODO:  maybe dump?? "EOF end of frame"
            //dump();
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


// StreamPipeline::StreamPipeline(Img_t& sImg) : Pipeline(sImg){
//     // nothing to do
// }

// StreamPipeline::~StreamPipeline(){
//     // nothing to do, and does not need to explicitly call the destructor from base class
// }

// void StreamPipeline::update_module_args(int frameInd){
//     std::list<Module_t>::iterator it;
//     for (it = _pipe.begin(); it != _pipe.end(); ++it){
//         //(*it).pArg = 
//         // TODO: two ways to update args. one is c-model simulation mode, just set values when module runs;
//         // the other is to read from a series of toml files for every frame, and override the values set by module.
//     }
// }

// void StreamPipeline::frames_run_pipe(PipeArgs_t& sArgs){
//     for(int i = _startFrameInd; i < _startFrameInd + _frameNum; ++i){
//         update_module_args(i); // should consider behavioral consistency with Pipeline, when frameNum = 1: RawStream has update_module_args but Pipeline does not.
//         run_pipe(sArgs);
//         // if config specifies what arguments for a specific frame, just use config
//         // if config not specified, use adapted
//         // if none of the above, keep unchanged
//     }
// }

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

    DWTArg_t sDWTArg;
    sDWTArg.level = 2;
    sDWTArg.orient = TWO_DIMENSIONAL;
    sDWTArg.inImgPanelId = 0;
    sDWTArg.outImgPanelId = 0;
    config_dwt_kernels_LeGall53<int16_t>(&sDWTArg, MIRROR);

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
    graph[4] = {DUMMY4, {DUMMY6}};
    graph[5] = {DUMMY5, {DUMMY6}};
    graph[6] = {DUMMY6, {DUMMY8}};
    graph[7] = {DUMMY7, {DUMMY6}}; 
    graph[8] = {DUMMY8, {}};

    Orders_t orders(2);
    orders[0] = {DUMMY3, {DUMMY1, DUMMY2}};
    orders[1] = {DUMMY6, {DUMMY4, DUMMY5, DUMMY7}};

    //Img_t* pInitInImg = (Img_t*)malloc(sizeof(Img_t));
    //construct_img(pInitInImg, RAW_RGGB, 4256, 2848, UNSIGNED, 16, 1, false); // can be anything if the first module in pipe is VIN, because VIN does not need InImg.
    
    Pipeline myPipe(graph, orders, true);
    myPipe.run_pipe(sArgs);

    // note:
    //Pipe_t pp = Pipe_t(4); // equivalent to Pipe_t pp(4);

}