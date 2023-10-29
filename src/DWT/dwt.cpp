#include <stdlib.h>
#include <iostream>
#include "dwt.hpp"
#include "../Infra/ImgDef.hpp"
#include "../Infra/RandImageGen.hpp"


// need to process a window (or a strip)
// may "crop" the input image 

// predict_step_1d(T data, padding_scheme), template typename T needed

// update_step_1d()


IMG_RTN_CODE dwt_forward_1d(Img_t* pInImg, Img_t* pOutImg, void* pDWTArg){
    
    return SUCCEED;
}




void test_dwt(){
 
    std::srand(std::time(NULL)); // only need once (maybe for each distribution). how to encapsulate?

    Img_t* pMyImg = NULL; // initialze
    pMyImg =(Img_t*)malloc(sizeof(Img_t));
    IMAGE_FMT imageFormat = RGB;
    size_t width = 50;
    size_t height = 30;
    size_t bitDepth = 16;
    size_t alignment = 32;
    if (construct_img(pMyImg, 
                imageFormat,
                width,
                height,
                bitDepth,
                alignment,
                true) == SUCCEED) {
        std::cout<<"ok\n";
    }

    view_img_properties(pMyImg);


    Distrib_t sDistrib = {0, 511, 3, 15};  
    ValCfg_t sValCfg = {false, rand_num_uniform, sDistrib};
    //ValCfg_t sValCfg = {false, constant_num, sDistrib};

    set_value(pMyImg, sValCfg);


    for (int i = 0; i < 12; i++){
        std::cout<<"    "<< (*((uint16_t*)(pMyImg->pImageData[0]) + i));
    }
    std::cout<<'\n';

    for (int i = 0; i < 12; i++){
        std::cout<<"    "<< (*((uint16_t*)(pMyImg->pImageData[0]) + pMyImg->strides[0] + i));
    }
    std::cout<<'\n';


    destruct_img(&pMyImg);

}