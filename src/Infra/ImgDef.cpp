
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
        case YUV420:{
            return "YUV420";
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
    std::cout<<"addr of pImageData = ";
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


IMG_RTN_CODE allocate_image_data(Img_t* pImg){
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
            assert(pImg->pImageData[0] == NULL); // because we set it to NULL in construct_img().
            pImg->pImageData[0] = (uint8_t*)malloc(pImg->height * pImg->strides[0]);
            if (pImg->pImageData[0] == NULL){
                return ALLOCATION_FAIL;
            }
            break;
        }
        case RGB:{
            for (int c = 0; c < 3; c++){
                assert(pImg->pImageData[c] == NULL); // because we set it to NULL in construct_img().
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

IMG_RTN_CODE construct_img( Img_t* pImg, 
                            const IMAGE_FMT imageFormat,
                            const size_t width,
                            const size_t height,
                            const size_t bitDepth,
                            const size_t alignment,
                            const bool allocateImage){
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
    if (allocateImage){
        if (allocate_image_data(pImg) != SUCCEED){
            std::cout<<"image is not successfully allocated.\n";
            return ALLOCATION_FAIL;
        }
    }
    return SUCCEED;
}

IMG_RTN_CODE ducplicate_img(const Img_t* pSrcImg, Img_t* pDstImg){
    // dose NOT set ROI because ROI size is in pixel (each pixel may take 1, 2 or 4 bytes), 
    // but the Img_t cannot handle different data types.
    if (pSrcImg == NULL){
        std::cout<<"source Img_t object is not allocated.\n";
        return ALLOCATION_FAIL;
    }
    if (pSrcImg->pImageData[0] == NULL){
        std::cout<<"source image is not reachable, maybe it is not allocated.\n";
        return ALLOCATION_FAIL;
    }
    if (pDstImg == NULL){
        std::cout<<"destination Img_t object is not allocated.\n";
        return ALLOCATION_FAIL;
    }
    if (construct_img(pDstImg, 
                    pSrcImg->imageFormat, 
                    pSrcImg->width,
                    pSrcImg->height,
                    pSrcImg->bitDepth,
                    pSrcImg->alignment,
                    true) == SUCCEED){
        // copy image data
        for (int c = 0; c < MAX_NUM_P; ++c){
            if (pSrcImg->pImageData[0] != NULL){
                for (int i = 0; i < pSrcImg->height; ++i){
                    for (int j = 0; j < pSrcImg->strides[c]; ++j){
                        *(pDstImg->pImageData[c] + i * (pSrcImg->strides[c]) + j) = *(pSrcImg->pImageData[c] + i * (pSrcImg->strides[c]) + j);
                    }
                }
            }
        }
    }
    else{
        std::cout<<"destination image not allocated successfully.\n";
        return ALLOCATION_FAIL;
    }
    return SUCCEED;
}

bool is_image_equal(const Img_t* pSrcImg, const Img_t* pDstImg){
    for (int c = 0; c < MAX_NUM_P; ++c){
        if (pSrcImg->pImageData[0] != NULL){
            for (int i = 0; i < pSrcImg->height; ++i){
                for (int j = 0; j < pSrcImg->strides[c]; ++j){
                    if(*(pDstImg->pImageData[c] + i * (pSrcImg->strides[c]) + j) != *(pSrcImg->pImageData[c] + i * (pSrcImg->strides[c]) + j)){
                        return false;
                    }
                }
            }
        }
    }
    return true;
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