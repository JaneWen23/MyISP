#include "pipeline.hpp"
#include "../Modules/Infra/ImageIO.hpp"
#include "../ThirdParty/OpenCV.hpp"

const char* get_module_name(MODULE_ENUM m){
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

Pipeline::Pipeline(int startFrameInd, int frameNum, Img_t& sImg){
    _startFrameInd = startFrameInd;
    _frameNum = frameNum;
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

void Pipeline::add_module_to_pipe(Module_t& sModule){
    _pipe.push_back(sModule);
}

void Pipeline::print_pipe(){
    std::list<Module_t>::iterator it;
    for (it = _pipe.begin(); it != _pipe.end(); ++it){
        std::cout<< get_module_name((*it).moduleEnum)<<"\n";
    }
}

void Pipeline::init_pipe(){
    // init all modules in the pipeline:
    
    // modules are configured ahead of pipeline


    // first we should add module to pipe, then we can init pipe
    //
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
    // if pipe is not initialized, should not run.
    std::list<Module_t>::iterator it;
    for (it = _pipe.begin(); it != _pipe.end(); ++it){
        set_in_img_t(*it);
        move_data();
        (*it).run_function(&_sInImg, &_sOutImg, (*it).pArg);
        // maybe dump??
        dump();
    }
}

void Pipeline::dump(){
    cv::Mat image;
    convert_img_t_to_cv_mat(image, &_sOutImg);
    imwrite("../dump/xxx.png", image); // TODO: file name??
}

// for each frame:
// run_pipe()
// update _frameInd, update sVinArg.frameInd

void test_pipeline(){
    ReadRawArg_t sVinArg = {
        "../data/rawData.raw", //const char* path;
        0, //int frameInd; // read i-th frame, i >= 0, WILL BE UPDATED IN THE RUN-TIME
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

    Img_t* pInitImg = (Img_t*)malloc(sizeof(Img_t));
    construct_img(pInitImg, RAW_RGGB, 4256, 2848, UNSIGNED, 16, 1, false);
    Pipeline myPipe(0, 1, *pInitImg);
    myPipe.add_module_to_pipe(sVinModule); // TODO: should be a cfg file to tell what modules to add
    myPipe.print_pipe();
    myPipe.run_pipe();
    destruct_img(&pInitImg);
}