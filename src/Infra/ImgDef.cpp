
#include <iostream>
#include <stdlib.h>
#include "ImgDef.hpp"


Img_t* init_img(){
    // allocate memory
    Img_t* pImg = NULL;
    pImg =(Img_t*)malloc(sizeof(Img_t*));
    if (pImg == NULL){
        std::cout<<"fail to allocate Img_t object\n";
        return NULL;
    }
    return pImg;
}

IMG_RTN_CODE construct_img(Img_t* pImg){
    // takes in args to set up the properties of the img type.
    if (pImg == NULL){
        return ALLOCATION_FAIL;
    }

    return SUCCEED;
}

IMG_RTN_CODE destruct_img(Img_t* pImg){
    return SUCCEED;
}

void test_img_def(){
    // should use allocate_image().
    Img_t* pMyImg = NULL; // initialze
    pMyImg =(Img_t*)malloc(sizeof(Img_t*));
    if (pMyImg == NULL){
        std::cout<<"fail to allocate Img_t object\n";
        return;
    }
    pMyImg->bitDepth = 10;
    pMyImg->height = 30;
    pMyImg->width = 50;
    pMyImg->nComp = 3;
    pMyImg->stride[0] = 100; // 50 * 2(bytes)
    pMyImg->stride[1] = 100;
    pMyImg->stride[2] = 100;
    pMyImg->stride[3] = 0; // because we only have 3 channels
    
    int* pImgData = NULL;
    pImgData = (int*)malloc(pMyImg->width * pMyImg->height * ((pMyImg->bitDepth+7)/8));
    if (pMyImg == NULL){
    std::cout<<"fail to allocat image data\n";
    return;
    }

    std::cout<<pMyImg<<'\n';
}