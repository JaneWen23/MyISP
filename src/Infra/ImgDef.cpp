
#include <iostream>
#include <stdlib.h>
#include "ImgDef.hpp"


const size_t get_next_multiple(const size_t x, const size_t m){
    // m is the multiple number (base number), and should be >= 1.
    // x is the number to find its next multiple of m
    assert(m > 0);
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


const char* get_image_format_name(const IMAGE_FMT imageFormat){
    switch (imageFormat){
        case MONO:{
            return "MONO";
            break;
        }    
        case RGB:{
            return "RGB";
            break;
        }
        case RAW_RGGB:{
            return "RAW_RGGB";
            break;
        }
        case RAW_GRGB:{
            return "RAW_GRGB";
            break;
        }
        case RAW_GBGR:{
            return "RAW_GBGR";
            break;
        }
        case RAW_BGGR:{
            return "RAW_BGGR";
            break;
        }
        default:{
            return "Unknown";
            break;
        }
    }
}


void view_img_properties(const Img_t* pImg){
    if (pImg == NULL){
        std::cout<<"Cannot view because Img_t object is not allocated.\n";
        return;
    }
    std::cout<<"imageFormat = "<< get_image_format_name(pImg->imageFormat)<<"\n";
    std::cout<<"width = "<< pImg->width<<"\n";
    std::cout<<"height = "<< pImg->height<<"\n";
    std::cout<<"bitDepth = "<< pImg->bitDepth<<"\n";
    std::cout<<"alignment = "<< pImg->alignment<<"\n";
    std::cout<<"strides = ";
    for (int c = 0 ; c < MAX_NUM_P; c++){
        std::cout<<pImg->strides[c]<<"; ";
    }
    std::cout<<"\n";
    std::cout<<"address of pImageData = ";
    for (int c = 0 ; c < MAX_NUM_P; c++){
        if (pImg->pImageData[c] != NULL){
            std::cout<< static_cast<const void*>(pImg->pImageData[c]) <<"; ";
        }
        else{
            std::cout<<"NULL; ";
        }
    }
    std::cout<<"\n";
}


void set_strides(Img_t* pImg){
    // strides are initiated by zeros.
    for (int c = 0; c < MAX_NUM_P; c++){
        pImg->strides[c] = 0;
    }  

    // if panel_0 width actual is 100, while panel_1 actual width is 50, what will be the strides?
    // the width will be 100 for both panels, and it is only a number, it does not affect actuall image allocation
    // so stride[0] = 100*num_of_bytes_for_a_pixel + make_up_alingment_bytes, 
    // and stride[1] = 50*num_of_bytes_for_a_pixel + make_up_alignment_bytes
    size_t bytes_per_line = get_next_multiple(pImg->width * (pImg->bitDepth + 7)/8, pImg->alignment);
    switch (pImg->imageFormat){
        case MONO:
        case RAW_RGGB:
        case RAW_GRGB:
        case RAW_GBGR:
        case RAW_BGGR:{
            pImg->strides[0] = bytes_per_line;
            break;
        }
        case RGB:{
            pImg->strides[0] = pImg->strides[1] = pImg->strides[2] = bytes_per_line;
            break;
        }
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
        std::cout<<"Img_t object is not allocated.\n";
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
    set_strides(pImg);
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
    switch(pImg->imageFormat){
        case MONO:
        case RAW_RGGB:
        case RAW_GRGB:
        case RAW_GBGR:
        case RAW_BGGR:{
            pImg->pImageData[0] = (uint8_t*)malloc(pImg->height * pImg->strides[0]);
            if (pImg->pImageData[0] == NULL){
                return ALLOCATION_FAIL;
            }
            break;
        }
        case RGB:{
            for (int c = 0; c < 3; c++){
                pImg->pImageData[c] = (uint8_t*)malloc(pImg->height * pImg->strides[c]);
                if (pImg->pImageData[c] == NULL){
                    return ALLOCATION_FAIL;
                }
            }
            break;
        }
        default:
            return ALLOCATION_FAIL;
    }
    return SUCCEED;
}


IMG_RTN_CODE free_image_data(Img_t* pImg){
    for (int c = 0; c < MAX_NUM_P; c++){
        if (pImg->pImageData[c] != NULL){
            free(pImg->pImageData[c]);
            pImg->pImageData[c] = NULL;
        }
    }
    return SUCCEED; // always succeed?
}

IMG_RTN_CODE destruct_img(Img_t** ptr_pImg){
    // this is different from "free_image_data(Img_t* pImg)", because to free the "image
    // pointer" needs to find its address taking use of pImg, and we do not change pImg.
    // when we want to free pImg itself, we want to free the argument, not only the parameter.
    // so we need another pointer(i.e., pointer to pointer) to "reflect to" the address of pImg.
    assert(ptr_pImg != NULL);
    free_image_data(*ptr_pImg);
    free(*ptr_pImg);
    *ptr_pImg = NULL;
    return SUCCEED; // always succeed?
}

// TODO: add func: crop_image(), not to copy and paset, but only to read original image with different offset
// addr_next_sart = offset(starting byte) + bytes_per_line

// IMG_RTN_CODE crop_image(Img_t* pImg, ){

// }


//TODO: initialize_image_data(Img_t* pImg, TYPE option); option can be "set to zero", or "set to random", with configurable range and distribution


//TODO: padding_scheme()


void test_img_def(){
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
                alignment) == SUCCEED) {
        std::cout<<"ok\n";
    }
    allocate_img_data(pMyImg);
    view_img_properties(pMyImg);
    destruct_img(&pMyImg);
    view_img_properties(pMyImg);


    size_t x = 50;
    size_t m = 16;
    size_t q = 0;
    std::cout<< get_next_multiple(x, m) <<'\n';

 }