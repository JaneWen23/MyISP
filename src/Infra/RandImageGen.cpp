#include <iostream>
#include <stdlib.h>
#include <random>
#include "RandImageGen.hpp"


const int constant_num(const Distrib_t& sDistrib){
    return sDistrib.lb;
}


const int rand_num_uniform(const Distrib_t& sDistrib){
    return std::rand()%(sDistrib.ub - sDistrib.lb + 1) + sDistrib.lb;
}


const int rand_num_gaussian(const Distrib_t& sDistrib){
    // should also take care of the min and max values allowed (or, range).
    // std::default_random_engine generator(time(NULL));
    // std::normal_distribution<int> distribution(sDistrib.mu, sDistrib.sigma);
    // return distribution(generator);
    return 0;
}


const int val_generated(const ValCfg_t& sValCfg){
    return sValCfg.FGetNum(sValCfg.sDistrib);
}


template<typename T>
void set_value_by_panel(Img_t* pImg, const ValCfg_t& sValCfg, const int panel, const int h, const int w){
    T* pTmp = (T*)(pImg->pImageData[panel]);
    int strideInPix = pImg->strides[panel] / sizeof(T);
    for (int i = 0; i < h; ++i){
        for (int j = 0; j < w; ++j){
            *(pTmp + i * strideInPix + j) = (T)val_generated(sValCfg);
        }
    }
}

typedef void (*FP)(Img_t*, const ValCfg_t&, const int, const int, const int);

IMG_RTN_CODE set_value(Img_t* pImg, const ValCfg_t& sValCfg){
    assert(pImg != NULL);
    // void (*funcPtrU8)(Img_t*, const ValCfg_t&, const int, const int, const int) = &set_value_by_panel<uint8_t>;
    // void (*funcPtrU16)(Img_t*, const ValCfg_t&, const int, const int, const int) = &set_value_by_panel<uint16_t>;
    // void (*funcPtrU32)(Img_t*, const ValCfg_t&, const int, const int, const int) = &set_value_by_panel<uint32_t>;

    FP f = NULL;

    if (sValCfg.sign == UNSIGNED){
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

    return SUCCEED;
}


void test_rand_image_gen(){
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
    ValCfg_t sValCfg = {UNSIGNED, rand_num_uniform, sDistrib};
    //ValCfg_t sValCfg = {UNSIGNED, constant_num, sDistrib};

    set_value(pMyImg, sValCfg);

    for (int i = 0; i < 12; i++){
        std::cout<<"    "<< (*((uint16_t*)(pMyImg->pImageData[0]) + i));
    }
    std::cout<<'\n';

    for (int i = 0; i < 12; i++){
        std::cout<<"    "<< (*((uint16_t*)(pMyImg->pImageData[0]) + pMyImg->strides[0] + i));
    }
    std::cout<<"\n\n\n";

    Img_t* pMyImg2 =(Img_t*)malloc(sizeof(Img_t));
    ducplicate_img(pMyImg, pMyImg2);

    for (int i = 0; i < 12; i++){
        std::cout<<"    "<< (*((uint16_t*)(pMyImg2->pImageData[0]) + i));
    }
    std::cout<<'\n';

    for (int i = 0; i < 12; i++){
        std::cout<<"    "<< (*((uint16_t*)(pMyImg2->pImageData[0]) + pMyImg2->strides[0] + i));
    }
    std::cout<<'\n';

    std::cout<<"is equal = "<< is_image_equal(pMyImg, pMyImg2)<<"\n";

    destruct_img(&pMyImg);
    destruct_img(&pMyImg2);
    
 }