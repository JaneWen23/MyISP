#include <iostream>
#include <stdlib.h>
#include "RandImageGen.hpp"

//TODO: RandImageGen.cpp, with configurable range and distribution, and fit with image format (bit depth, height, width)


void test_rand_image_gen(){
   Img_t* pMyImg = NULL; // initialze
    pMyImg =(Img_t*)malloc(sizeof(Img_t));
    IMAGE_FMT imageFormat = RGB;
    size_t width = 50;
    size_t height = 30;
    size_t bitDepth = 16;
    size_t alignment = 16;
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

    // try to use the image:
    uint16_t* pMyData_p0 = NULL;
    pMyData_p0 = (uint16_t*)(pMyImg->pImageData[0]);
    if (pMyData_p0 == NULL){
        return;
    }
    for (int i = 0; i < pMyImg->height; ++i){
        for (int j = 0; j < pMyImg->width; ++j){
            *(pMyData_p0 + i * (pMyImg->strides[0]) + j) = i * pMyImg->height + j + 34;
        }
    }

    for (int i = 0; i < 12; i++){
        std::cout<<"    "<< (*(pMyData_p0 + i));
    }
    std::cout<<'\n';

    for (int i = 0; i < 12; i++){
        std::cout<<"    "<< (*(pMyData_p0 + pMyImg->strides[0] + i));
    }
    std::cout<<'\n';

    destruct_img(&pMyImg);

    // but we have not null-ed pMyData_p0 yet. this is dangerous.
    pMyData_p0 = NULL;
    
 }