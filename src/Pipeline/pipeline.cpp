#include "pipeline.hpp"
#include "../Modules/Infra/ImageIO.hpp"

Pipeline::Pipeline(int startFrameInd, int frameNum){
    _startFrameInd = startFrameInd;
    _frameNum = frameNum;
    for(int c = 0; c < MAX_NUM_P; ++c){
        _sInImg.pImageData[c] = NULL;
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

void Pipeline::print_pipe(){
    std::list<Module_t>::iterator it;
    for (it = _pipe.begin(); it != _pipe.end(); ++it){
        std::cout<< get_module_name((*it).moduleEnum)<<"\n";
    }
}

void test_pipeline(){
    ReadRawArg_t sVinArg = {
        "../data/rawData.raw", //const char* path;
        0, //int frameInd; // read i-th frame, i >= 0
        RAW_RGGB, //IMAGE_FMT imageFormat;
        4256, //int width;
        2848, //int height;
        16, //int bitDepth;
        1 //int alignment;
    };

    ReadRawArg_t* pVinArg = &sVinArg;
    Img_t* pOutImg = (Img_t*)malloc(sizeof(Img_t));
    // read_raw_frame(NULL, pOutImg, pVinArg);
    // view_img_properties(pOutImg);
    

    Module_t sModule ={
        ISP_VIN,
        RAW_RGGB,
        RAW_RGGB,
        16,
        16,
        UNSIGNED,
        UNSIGNED,
        (void*)pVinArg
    };

    Pipeline myPipe(0, 1);
    myPipe.add_module_to_pipe(sModule);
    myPipe.print_pipe();
}