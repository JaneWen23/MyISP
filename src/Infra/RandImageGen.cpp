#include <iostream>
#include <stdlib.h>
#include "RandImageGen.hpp"


// image format is one aspect; bit depth is another aspect.
// according to bitdepth (and format), determine signed or unsigned, data type, and number of panels
// such as int16, or uint16
// then create the image pointer
// then convert the original imagedata pointer this this type


// then set values to the image (take care of number of panels, and different width and height for different panels)
// after everything is finished, destruct pImg
// DO NOT FORGET to set to the image pointer to NULL as well.


int val_generated(const Val_t& sVal){
    // how to deal with the range, mean, std dev?
    // make option as a struct. can pass by reference, since it cannot be null.
    return sVal.constant;
}

// to be removed:
// to be made into template
// void set_value_u8(Img_t* pImg, const int option, const int panel, const int h, const int w){
//     uint8_t* pTmp = pImg->pImageData[panel];
//     for (int i = 0; i < h; ++i){
//         for (int j = 0; j < w; ++j){
//             *(pTmp + i * (pImg->strides[panel]) + j) = (uint8_t)val_generated(option);
//         }
//     }
// }

template<typename T>
void set_value_by_panel(Img_t* pImg, const Val_t& sVal, const int panel, const int h, const int w){
    T* pTmp = (T*)(pImg->pImageData[panel]);
    for (int i = 0; i < h; ++i){
        for (int j = 0; j < w; ++j){
            *(pTmp + i * (pImg->strides[panel]) + j) = (T)val_generated(sVal);
        }
    }
}

typedef void (*FP)(Img_t*, const Val_t&, const int, const int, const int);

void set_value(Img_t* pImg, Val_t& sVal){
    assert(pImg != NULL);
    // void (*funcPtrU8)(Img_t*, const Val_t&, const int, const int, const int) = &set_value_by_panel<uint8_t>;
    // void (*funcPtrU16)(Img_t*, const Val_t&, const int, const int, const int) = &set_value_by_panel<uint16_t>;
    // void (*funcPtrU32)(Img_t*, const Val_t&, const int, const int, const int) = &set_value_by_panel<uint32_t>;

    FP f = NULL;

    if (sVal.isSigned == false){
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
        // to be finished...
    }

    // if not YUV:
    for (int c = 0; c < MAX_NUM_P; c++){
        if (pImg->pImageData[c] != NULL){
            f(pImg, sVal, c, pImg->height, pImg->width);
        }
    }
    // YUV: different panels may have different height and width
}

// random, uniformly distributed, with range configurable

// random, guassian distributed, with mean and deviation configurable

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

    Val_t sVal = {false, 2};

    set_value(pMyImg, sVal);


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