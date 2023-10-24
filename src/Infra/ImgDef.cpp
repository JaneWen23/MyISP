
#include <iostream>
#include <stdlib.h>
#include "ImgDef.hpp"


const size_t get_next_multiple(const size_t x, const size_t m){
    // m is the multiple number (base number), and should be >= 1.
    // x is the number to find its next multiple of m
    if (m < 1){
        std::cout<< "invalid input, base number m cannot be less than 1.\n";
        return 0; // @todo: @throws: do not want to return but throw out an error. maybe assert??
    }
    if ((m & (m - 1)) == 0){
        // if m > 0 and m is power of 2, use this simplified algo.
        return ((x + m -1) & ~(m - 1));
    }
    else{       
        size_t q = 0;
        q = x%m;
        return x + m * (bool)q - q;
    }
}

void view_img_properties(Img_t* pImg){
    std::cout<<"imageFormat = "<< pImg->imageFormat<<"\n";
    std::cout<<"width = "<< pImg->width<<"\n";
    std::cout<<"height = "<< pImg->height<<"\n";
    std::cout<<"nComp = "<< pImg->nComp<<"\n";
    std::cout<<"bitDepth = "<< pImg->bitDepth<<"\n";
    std::cout<<"alignment = "<< pImg->alignment<<"\n";
    std::cout<<"strides = ";
    for (int c = 0 ; c < MAX_NUM_P - 1; c++){
        std::cout<<pImg->strides[c]<<", ";
    }
    std::cout<<pImg->strides[MAX_NUM_P-1]<<"\n";
    // std::cout<<"Address of pImageData = ";
    // for (int c = 0 ; c < MAX_NUM_P - 1; c++){
    //     std::cout<<pImg->pImageData[c]<<", ";
    // }
    // std::cout<<pImg->pImageData[MAX_NUM_P-1]<<"\n";
}

void set_panels_strides(Img_t* pImg){
    // strides are initiated by zeros.
    pImg->nComp = 0;
    for (int c = 0; c < MAX_NUM_P; c++){
        pImg->strides[c] = 0;
    }  

    // if panel_0 width actual is 100, while panel_1 actual width is 50, what will be the strides?
    // the width will be 100 for both panels, and it is only a number, it does not affect actuall image allocation
    // so stride[0] = 100*(bitDepth+7)/8 + make_up_alingment_bytes, 
    // and stride[1] = 50*(bitDepth+7)/8 + make_up_alignment_bytes
    size_t bytes_per_line = get_next_multiple(pImg->width * (pImg->bitDepth + 7)/8, pImg->alignment);
    switch (pImg->imageFormat){
        case MONO:
        case RAW_RGGB:
        case RAW_GRGB:
        case RAW_GBGR:
        case RAW_BGGR:
            pImg->nComp = 1;
            pImg->strides[0] = bytes_per_line;
            break;
        case RGB:
            pImg->nComp = 3;
            pImg->strides[0] = pImg->strides[1] = pImg->strides[2] = bytes_per_line;
            break;
        default:
            break;
    }
}   


IMG_RTN_CODE construct_img( Img_t* pImg, 
                            IMAGE_FMT imageFormat,
                            size_t width,
                            size_t height,
                            size_t bitDepth,
                            size_t alignment){
    // takes in args to set up the properties of the img type; 
    if (pImg == NULL){
        std::cout<<"fail to allocate Img_t object\n";
        return ALLOCATION_FAIL;
    }
    pImg->imageFormat = imageFormat;
    pImg->width = width; // but there may be a different widt and/or height in a different channel, what we do is to control the stride
    pImg->height = height;
    pImg->bitDepth = bitDepth;
    pImg->alignment = alignment;
 
    if (alignment == 0){
        std::cout<<" alignment should be >= 1 but got 0 instead.\n";
        return INVALID_INPUT;
    }
    set_panels_strides(pImg);
    for (int c = 0; c < MAX_NUM_P; c++){
        pImg->pImageData[c] = NULL;
    }  
     
    return SUCCEED;
}



IMG_RTN_CODE allocate_img_data(Img_t* pImg){
    // allocate memory for image data
    if (pImg == NULL){
        std::cout<<"the input arg pImg is null\n";
        return INVALID_INPUT;
    }

    for (int c = 0; c < pImg->nComp; c++){
        pImg->pImageData[c] = (uint8_t*)malloc(pImg->strides[c]);
        if (pImg->pImageData[c] == NULL){
            return ALLOCATION_FAIL;
        }
    }
    return SUCCEED;
}

IMG_RTN_CODE free_image_data(Img_t* pImg){
    for (int c = 0; c < pImg->nComp; c++){
        if (pImg->pImageData[c] != NULL){
            free(pImg->pImageData[c]);
            pImg->pImageData[c] = NULL;
        }
    }
    return SUCCEED; // always succeed?
}

IMG_RTN_CODE destruct_img(Img_t* pImg){
    if (pImg != NULL){
        free_image_data(pImg);
        free(pImg);
        pImg = NULL;
    }
    return SUCCEED; // always succeed?
}

// @todo: add func: crop_image(), not to copy and paset, but only to read original image with different stride
// addr_next_sart = offset(starting byte) + bytes_per_line

void test_img_def(){
    Img_t* pMyImg = NULL; // initialze
    pMyImg =(Img_t*)malloc(sizeof(Img_t*));
    IMAGE_FMT imageFormat = RGB;
    size_t width = 50;
    size_t height = 30;
    size_t bitDepth = 10;
    size_t alignment = 16;
    if (construct_img(pMyImg, 
                imageFormat,
                width,
                height,
                bitDepth,
                alignment) == SUCCEED) 
    {
        view_img_properties(pMyImg);
        std::cout<<"ok\n";
    }


    size_t x = 50;
    size_t m = 16;
    size_t q = 0;
    std::cout<< get_next_multiple(x, m) <<'\n';
    // q = x%m;
    // std::cout<< (x + m * (bool)q - q) <<'\n';

    //std::cout<< ((16 & (15))==0) <<'\n';
 }