#include "pipeline.hpp"
#include "../ThirdParty/OpenCV.hpp"

const char* get_module_name(const MODULE_ENUM m){
    switch(m){
        case ISP_VIN:{
            return "ISP_VIN";
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
    }
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
        std::cout<< get_module_name((*it).moduleEnum)<<"\n";
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

void Pipeline::run_pipe(){
    if (!_pipe.empty()){
        std::list<Module_t>::iterator it;
        for (it = _pipe.begin(); it != _pipe.end(); ++it){
            set_in_img_t(*it);
            move_data();
            (*it).run_function(&_sInImg, &_sOutImg, (*it).pArg);
            // then check if out img types are consistent with sModule outs'
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
    }
}

void StreamPipeline::frames_run_pipe(){
    for(int i = _startFrameInd; i < _startFrameInd + _frameNum; ++i){
        update_module_args(i); // should consider behavioral consistency with Pipeline, when frameNum = 1: RawStream has update_module_args but Pipeline does not.
        run_pipe();
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

    ReadRawArg_t* pVinArg = &sVinArg;
    
    Module_t sVinModule ={
        ISP_VIN,
        RAW_RGGB,
        RAW_RGGB,
        16,
        16,
        UNSIGNED,
        UNSIGNED,
        (void*)pVinArg,
        read_raw_frame
    };

    Img_t* pInitInImg = (Img_t*)malloc(sizeof(Img_t));
    construct_img(pInitInImg, RAW_RGGB, 4256, 2848, UNSIGNED, 16, 1, false); // can be anything if the first module in pipe is VIN, because VIN does not need InImg.
    Pipeline myPipe(*pInitInImg);
    myPipe.add_module_to_pipe(sVinModule); // TODO: should be a cfg file to tell what modules to add // this is "config pipe"; also need to config frames and loops
    // maybe we should add a function to generate ReadRawArg according to cfg
    // and things to generate ReadRawArg will be the Vin arg
    // i.e., ReadRaw is wrapped inside Vin
    // ReadRawArg is just a private struct and need not to known by pipeline
    // Vin now is related to the frames, i.e., higher level than pipeline
    // 
    myPipe.print_pipe();
    myPipe.run_pipe();
    // pipe update config (update sModule.pArg)
    // then run pipe again
    destruct_img(&pInitInImg);
}
// different loops: use different module arg configs, but same pipe
// config may be nested
// loop {frame [module serial]}
// or, no loop, just frame; loop is treated like another object so it needs another config