#include "pipeline.hpp"
#include <algorithm>
#include <iostream>

void make_pipe(const Graph_t& graph, const MODULE_NAME* sorted, Pipe_t& pipe){
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

    // first check if predecessors is subset of cfg-provided,
    // then check if cfg-provided is subset of predecessors,
    // if both are true, replace predecessors with cfg-provided,
    // if not, report an error and exit.


}


void print_pipe(Pipe_t& pipe){
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

void* find_arg_for_func(AllArgs_t& sArgs, MODULE_NAME m){
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
            return &(sArgs.sDummyArg);
        }
        case ISP_VIN:{
            return &(sArgs.sVinArg);
        }
        case ISP_COMPRESSION:{
            return &(sArgs.sCompressionArg);
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
            return &(sArgs.sCCMArg);
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

Pipeline::Pipeline(const Graph_t& graph, bool needPrint){
    // initialize _sorted and _pipe according to number of nodes:
    int n = graph.size();
    _sorted = (MODULE_NAME*)malloc( n * sizeof(MODULE_NAME));
    _pipe = Pipe_t(n); // TODO: is it constructor of std::vector<T> ??? why TYPE(n) ????
    // generate pipeline from graph:
    topological_sort(graph, _sorted);
    make_pipe(graph, _sorted, _pipe);
    if (needPrint){
        print_pipe(_pipe);
    }
    _pOutImg = (PipeImg_t*) malloc(sizeof(PipeImg_t));
}

Pipeline::~Pipeline(){
    free(_sorted);
    // do not destruct _pipe manually; it was constructed in stack area.
    free(_pOutImg);

    // for(int c = 0; c < MAX_NUM_P; ++c){
    //     if(_sOutImg.pImageData[c] != NULL){
    //         free(_sOutImg.pImageData[c]);
    //         _sOutImg.pImageData[c] = NULL;
    //     }
    // }
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



ImgPtrs_t Pipeline::set_in_img_t(Module_t& sModule){
    ImgPtrs_t imgPtrs = {NULL};
    // consider module name and its predecessor modules
    // if there is(are) predecessor(s), the order main, addl1, addl2, ... is the same as the order of predecessors

    if (sModule.pred_modules.size() > 1){
        // find the MArg from AllArgs, then find MArg.order
    }
    else if (sModule.pred_modules.size() == 1){
        //imgPtrs.pMainImg = 
    }
    else if (sModule.pred_modules.size() == 0){
        return imgPtrs; // in this case, imgPtrs is dont-care for the module.
    }

    return imgPtrs;
}

void Pipeline::move_data(){

}

// void Pipeline::run_pipe(PipeArgs_t& sArgs){
//     if (!_pipe.empty()){
//         std::list<Module_t>::iterator it;
//         for (it = _pipe.begin(); it != _pipe.end(); ++it){
//             set_in_img_t(*it);
//             move_data();
//             (*it).run_function(_sInImgPtrs, &_sOutImg, find_arg_for_func(sArgs, (*it).module));
//             // TODO:  maybe dump?? "EOF end of frame"
//             dump();
//         }
//     }
//     else{
//         std::cout<<"warning: pipeline is empty. nothing processed.\n";
//     }
// }

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
    ReadRawArg_t sVinArg = {
        "../data/rawData.raw", //const char* path;
        0, //int frameInd; // read i-th frame, i >= 0, WILL BE UPDATED IN THE RUN-TIME; if rewind = true, this will not be updated
        RAW_RGGB, //IMAGE_FMT imageFormat;
        4256, //int width;
        2848, //int height;
        16, //int bitDepth;
        1, //int alignment;
    };

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

    CCMArg_t sCCMArg = {
        {278, -10, -8},
        {-12, 269, -8},
        {-10, -3, 272},
    };
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


    //Img_t* pInitInImg = (Img_t*)malloc(sizeof(Img_t));
    //construct_img(pInitInImg, RAW_RGGB, 4256, 2848, UNSIGNED, 16, 1, false); // can be anything if the first module in pipe is VIN, because VIN does not need InImg.
    
    Pipeline myPipe(graph, true);

    // note:
    //Pipe_t pp = Pipe_t(4); // equivalent to Pipe_t pp(4);

    std::vector<int> vv(8);
    for (int i = 0; i < vv.size(); ++i){
        vv[i] = i;
    }

    vv.erase(vv.begin()+2);

    for (int i = 0; i < vv.size(); ++i){
        std::cout<< vv[i] << ", ";
    }
    std::cout<<"\n";

}