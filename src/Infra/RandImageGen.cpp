#include <iostream>
#include <stdlib.h>
#include "RandImageGen.hpp"



// random, uniformly distributed, with range configurable
// random, guassian distributed, with mean and deviation configurable
int val_generated(const ValCfg_t& sValCfg){
    // how to deal with the range, mean, std dev?
    
    return sValCfg.constant; // should return a function expression, maybe.
}


template<typename T>
void set_value_by_panel(Img_t* pImg, const ValCfg_t& sValCfg, const int panel, const int h, const int w){
    T* pTmp = (T*)(pImg->pImageData[panel]);
    for (int i = 0; i < h; ++i){
        for (int j = 0; j < w; ++j){
            *(pTmp + i * (pImg->strides[panel]) + j) = (T)val_generated(sValCfg);
        }
    }
}

typedef void (*FP)(Img_t*, const ValCfg_t&, const int, const int, const int);

void set_value(Img_t* pImg, ValCfg_t& sValCfg){
    assert(pImg != NULL);
    // void (*funcPtrU8)(Img_t*, const ValCfg_t&, const int, const int, const int) = &set_value_by_panel<uint8_t>;
    // void (*funcPtrU16)(Img_t*, const ValCfg_t&, const int, const int, const int) = &set_value_by_panel<uint16_t>;
    // void (*funcPtrU32)(Img_t*, const ValCfg_t&, const int, const int, const int) = &set_value_by_panel<uint32_t>;

    FP f = NULL;

    if (sValCfg.isSigned == false){
        if (pImg->bitDepth <= 8){
            f = set_value_by_panel<uint8_t>;
        }
        else if (pImg->bitDepth <= 16){
            f = set_value_by_panel<uint16_t>;
        }
        else if (pImg->bitDepth <= 32){
            f = set_value_by_panel<uint32_t>;
        }
    }
    else {
        f = set_value_by_panel<int>;
    }

    // if not YUV:
    for (int c = 0; c < MAX_NUM_P; c++){
        if (pImg->pImageData[c] != NULL){
            f(pImg, sValCfg, c, pImg->height, pImg->width);
        }
    }
    // YUV: different panels may have different height and width
}


void test_rand_image_gen(){
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

    ValCfg_t sValCfg = {false, 2};

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