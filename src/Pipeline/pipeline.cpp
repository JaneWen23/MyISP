#include "pipeline.hpp"
#include "../ThirdParty/OpenCV.hpp"

const char* get_module_name(const MODULE_ENUM m){
    switch(m){
        case ISP_NONE:{
            return "ISP_NONE";
        }
        case ISP_VIN:{
            return "ISP_VIN";
        }
        case ISP_COMPRESSION:{
            return "ISP_COMPRESSION";
        }
        case ISP_BLC:{
            return "ISP_BLC";
        }
        case ISP_DMS:{
            return "ISP_DMS";
        }
        case ISP_WB:{
            return "ISP_WB";
        }
        case ISP_CCM:{
            return "ISP_CCM";
        }
        case ISP_RGB2YUV:{
            return "ISP_RGB2YUV";
        }
        default:{
            return "UNKNOWN ISP MODULE";
        }
    }
    return "";
}

void* find_arg_for_func(Args_t& sArgs, MODULE_ENUM m){
    switch (m){
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

Pipeline::Pipeline(Img_t& sImg){
    _sInImg = sImg;
    for(int c = 0; c < MAX_NUM_P; ++c){
        _sOutImg.pImageData[c] = NULL;
    }
}

Pipeline::~Pipeline(){
    for(int c = 0; c < MAX_NUM_P; ++c){
        if(_sInImg.pImageData[c] != NULL){
            free(_sInImg.pImageData[c]);
            _sInImg.pImageData[c] = NULL;
        }
    }
    for(int c = 0; c < MAX_NUM_P; ++c){
        if(_sOutImg.pImageData[c] != NULL){
            free(_sOutImg.pImageData[c]);
            _sOutImg.pImageData[c] = NULL;
        }
    }
}

bool Pipeline::is_pipe_valid_till_now(Module_t& sModule){
    // if _pipe is empty, nothing will go wrong
    if (!_pipe.empty()){
        if(_pipe.back().outBitDepth != sModule.inBitDepth){return false;}
        if(_pipe.back().outFmt != sModule.inFmt){return false;}
        if(_pipe.back().outSign != sModule.inSign){return false;}
    }
    return true;
}

void Pipeline::add_module_to_pipe(Module_t& sModule){
    if(is_pipe_valid_till_now(sModule)){
        _pipe.push_back(sModule);
    }
    else{
        std::cout<<"error: last module's output does not match with the input of current module. exited.\n";
        exit(1);
    }
}

void Pipeline::print_pipe(){
    std::list<Module_t>::iterator it;
    for (it = _pipe.begin(); it != _pipe.end(); ++it){
        std::cout<< get_module_name((*it).module)<<"\n";
    }
}

void Pipeline::set_in_img_t(Module_t& sModule){
    _sInImg.imageFormat = sModule.inFmt;
    _sInImg.sign = sModule.inSign;
    _sInImg.bitDepth = sModule.inBitDepth;
}

void Pipeline::move_data(){
    for (int c = 0; c < MAX_NUM_P; ++c){
        if (_sInImg.pImageData[c] != NULL){
            free(_sInImg.pImageData[c]); // if in img previously contains data, we should free it
            _sInImg.pImageData[c] = NULL;
        }
        _sInImg.pImageData[c] = _sOutImg.pImageData[c]; // deliver data from out to in
        _sOutImg.pImageData[c] = NULL; // out img should not contain data now
    }
}

void Pipeline::run_pipe(Args_t& sArgs){
    if (!_pipe.empty()){
        std::list<Module_t>::iterator it;
        for (it = _pipe.begin(); it != _pipe.end(); ++it){
            set_in_img_t(*it);
            move_data();
            (*it).run_function(&_sInImg, &_sOutImg, find_arg_for_func(sArgs, (*it).module));
            // TODO: then check if out img types are consistent with sModule outs'
            // TODO:  maybe dump?? "EOF end of frame"
            dump();
        }
    }
    else{
        std::cout<<"warning: pipeline is empty. nothing processed.\n";
    }
}

void Pipeline::dump(){
    cv::Mat image;
    convert_img_t_to_cv_mat(image, &_sOutImg);
    imwrite("../dump/xxx.png", image); // TODO: file name?? + prefix, protected; RawStreamPipeline should have its own prefix and inherit Pipeline's prefix
}


StreamPipeline::StreamPipeline(Img_t& sImg) : Pipeline(sImg){
    // nothing to do
}

StreamPipeline::~StreamPipeline(){
    // nothing to do, and does not need to explicitly call the destructor from base class
}

void StreamPipeline::update_module_args(int frameInd){
    std::list<Module_t>::iterator it;
    for (it = _pipe.begin(); it != _pipe.end(); ++it){
        //(*it).pArg = 
        // TODO: two ways to update args. one is c-model simulation mode, just set values when module runs;
        // the other is to read from a series of toml files for every frame, and override the values set by module.
    }
}

void StreamPipeline::frames_run_pipe(Args_t& sArgs){
    for(int i = _startFrameInd; i < _startFrameInd + _frameNum; ++i){
        update_module_args(i); // should consider behavioral consistency with Pipeline, when frameNum = 1: RawStream has update_module_args but Pipeline does not.
        run_pipe(sArgs);
        // if config specifies what arguments for a specific frame, just use config
        // if config not specified, use adapted
        // if none of the above, keep unchanged
    }
}

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

    MyJXSArg_t sCompressionArg = {sStarTetrixArg, sDWTArg};

    CCMArg_t sCCMArg = {
        {278, -10, -8},
        {-12, 269, -8},
        {-10, -3, 272},
    };
    //======================================================


    Args_t sArgs = {
        sVinArg,
        sCompressionArg,
        sCCMArg
    };
    
    Module_t sVinModule ={
        ISP_VIN,
        ISP_NONE,
        RAW_RGGB,
        RAW_RGGB,
        16,
        16,
        UNSIGNED,
        UNSIGNED,
        read_raw_frame
    };

    Module_t sCompressionModule ={
        ISP_COMPRESSION,
        ISP_VIN,
        RAW_RGGB,
        RAW_RGGB,
        16,
        16,
        UNSIGNED,
        SIGNED,
        my_jxs_pipe_sim
    };

    Img_t* pInitInImg = (Img_t*)malloc(sizeof(Img_t));
    construct_img(pInitInImg, RAW_RGGB, 4256, 2848, UNSIGNED, 16, 1, false); // can be anything if the first module in pipe is VIN, because VIN does not need InImg.
    Pipeline myPipe(*pInitInImg);
    myPipe.add_module_to_pipe(sVinModule);

    myPipe.print_pipe();
    myPipe.run_pipe(sArgs);

    destruct_img(&pInitInImg);
}