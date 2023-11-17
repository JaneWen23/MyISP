#include <stdlib.h>
#include <iostream>
#include "../Filter/SlidingWindow.hpp"
#include "../Math/Math.hpp"
#include "dwt.hpp"

#include "../Infra/RandImageGen.hpp"

template<typename T>
void dwt_horizontal_swap(uint8_t* pData, const int strideInPix, const int i, const int j){
    // at row i, swap pixels at col j and j + 1
    T tmp = 0;
    tmp = *((T*)pData + i*strideInPix + j);
    *((T*)pData + i*strideInPix + j) = *((T*)pData + i*strideInPix+ j + 1);
    *((T*)pData + i*strideInPix + j + 1) = tmp;
}

template<typename T>
void dwt_vertical_swap(uint8_t* pData, const int strideInPix, const int i, const int j){
    // at col j, swap pixels at row i and i + 1
    T tmp = 0;
    tmp = *((T*)pData + i*strideInPix + j);
    *((T*)pData + i*strideInPix + j) = *((T*)pData + (i+1)*strideInPix+ j);
    *((T*)pData + (i+1)*strideInPix+ j) = tmp;
}

typedef void (*FSWAP)(uint8_t*, const int, const int, const int);


void dwt_horizontal_reorder(Img_t* pImg, const ROI_t sImgROI){
    //  must begin with ind = 0, otherwise, there will be errors.
    FSWAP fswap = NULL;
    int scale = 0;
    if (pImg->bitDepth <= 8){
        scale = sizeof(int8_t);
        fswap = dwt_horizontal_swap<int8_t>;
    }
    else if (pImg->bitDepth <= 16){
        scale = sizeof(int16_t);
        fswap = dwt_horizontal_swap<int16_t>;
    }
    else if (pImg->bitDepth <= 32){
        scale = sizeof(int);
        fswap = dwt_horizontal_swap<int>;
    }
    assert(scale > 0);
    int strideInPix = pImg->strides[sImgROI.panelId] / scale;

    for (int i = sImgROI.startRow; i < sImgROI.startRow + sImgROI.roiHeight; ++i){
        for (int t = 1; t <= (sImgROI.roiWidth-1)>>1; ++t){
            for (int j = t; j < sImgROI.roiWidth - t; j += 2){
                fswap(pImg->pImageData[sImgROI.panelId], strideInPix, i, j);
            }
        }
    }
}

void dwt_vertical_reorder(Img_t* pImg, const ROI_t sImgROI){
    //  must begin with ind = 0, otherwise, there will be errors.
    FSWAP fswap = NULL;
    int scale = 0;
    if (pImg->bitDepth <= 8){
        scale = sizeof(int8_t);
        fswap = dwt_vertical_swap<int8_t>;
    }
    else if (pImg->bitDepth <= 16){
        scale = sizeof(int16_t);
        fswap = dwt_vertical_swap<int16_t>;
    }
    else if (pImg->bitDepth <= 32){
        scale = sizeof(int);
        fswap = dwt_vertical_swap<int>;
    }
    assert(scale > 0);
    int strideInPix = pImg->strides[sImgROI.panelId] / scale;

    for (int j = sImgROI.startCol; j < sImgROI.startCol + sImgROI.roiWidth; ++j){
        for (int t = 1; t <= (sImgROI.roiHeight-1)>>1; ++t){
            for (int i = t; i < sImgROI.roiHeight - t; i += 2){
                fswap(pImg->pImageData[sImgROI.panelId], strideInPix, i, j);
            }
        }
    }
}


void dwt_horizontal_reorder_back(Img_t* pImg, const ROI_t sImgROI){
    // must begin with ind = 0, otherwise, there will be errors.
    FSWAP fswap = NULL;
    int scale = 0;
    if (pImg->bitDepth <= 8){
        scale = sizeof(int8_t);
        fswap = dwt_horizontal_swap<int8_t>;
    }
    else if (pImg->bitDepth <= 16){
        scale = sizeof(int16_t);
        fswap = dwt_horizontal_swap<int16_t>;
    }
    else if (pImg->bitDepth <= 32){
        scale = sizeof(int);
        fswap = dwt_horizontal_swap<int>;
    }
    assert(scale > 0);
    int strideInPix = pImg->strides[sImgROI.panelId] / scale;

    for (int i = sImgROI.startRow; i < sImgROI.startRow + sImgROI.roiHeight; ++i){
        for (int t = (sImgROI.roiWidth-1)>>1; t > 0; --t){
            for (int j = t; j < sImgROI.roiWidth - t; j += 2){
                fswap(pImg->pImageData[sImgROI.panelId], strideInPix, i, j);
            }
        }
    }
}

void dwt_vertical_reorder_back(Img_t* pImg, const ROI_t sImgROI){
    //  must begin with ind = 0, otherwise, there will be errors.
    FSWAP fswap = NULL;
    int scale = 0;
    if (pImg->bitDepth <= 8){
        scale = sizeof(int8_t);
        fswap = dwt_vertical_swap<int8_t>;
    }
    else if (pImg->bitDepth <= 16){
        scale = sizeof(int16_t);
        fswap = dwt_vertical_swap<int16_t>;
    }
    else if (pImg->bitDepth <= 32){
        scale = sizeof(int);
        fswap = dwt_vertical_swap<int>;
    }
    assert(scale > 0);
    int strideInPix = pImg->strides[sImgROI.panelId] / scale;

    for (int j = sImgROI.startCol; j < sImgROI.startCol + sImgROI.roiWidth; ++j){
        for (int t = (sImgROI.roiHeight-1)>>1; t>0; --t){
            for (int i = t; i < sImgROI.roiHeight - t; i += 2){
                fswap(pImg->pImageData[sImgROI.panelId], strideInPix, i, j);
            }
        }
    }
}

template<typename T>
void config_kernels_horizontal_LeGall53(DWTArg_t* pDWTArg, const DIMENSION dimension, const int dwtLevel, const PADDING& padding){
    assert(dwtLevel > 0);
    pDWTArg->level = dwtLevel;
    pDWTArg->dim = dimension;

    pDWTArg->numLiftingSteps = 2; // defined by LeGall 5/3

    Formulas_T<T> sFml1; // scope!!! duration!!!
    sFml1.f = LeGall53_fwd_predict; // not related to orientation
    Formulas_T<T> sFml2;
    sFml2.f = LeGall53_fwd_update;
    Formulas_T<T> sFml3;
    sFml3.f = LeGall53_bwd_update;
    Formulas_T<T> sFml4;
    sFml4.f = LeGall53_bwd_predict;

    // horizontal
    KernelCfg_t sKernelCfg_fwd_hori_p = {
        NULL, 1, 3, 0, 0, padding, 2, 1, 2, 1, false, (void*)sFml1.f, false};
    KernelCfg_t sKernelCfg_fwd_hori_u = {
        NULL, 1, 3, 1, 0, padding, 2, 1, 2, 1, false, (void*)sFml2.f, false};
    KernelCfg_t sKernelCfg_bwd_hori_u = {
        NULL, 1, 3, 1, 0, padding, 2, 1, 2, 1, false, (void*)sFml3.f, false};
    KernelCfg_t sKernelCfg_bwd_hori_p = {
        NULL, 1, 3, 0, 0, padding, 2, 1, 2, 1, false, (void*)sFml4.f, false};

    // vertical
    KernelCfg_t sKernelCfg_fwd_vert_p = {
        NULL, 3, 1, 0, 0, padding, 1, 2, 1, 2, false, (void*)sFml1.f, false};
    KernelCfg_t sKernelCfg_fwd_vert_u = {
        NULL, 3, 1, 0, 1, padding, 1, 2, 1, 2, false, (void*)sFml2.f, false};
    KernelCfg_t sKernelCfg_bwd_vert_u = {
        NULL, 3, 1, 0, 1, padding, 1, 2, 1, 2, false, (void*)sFml3.f, false};
    KernelCfg_t sKernelCfg_bwd_vert_p = {
        NULL, 3, 1, 0, 0, padding, 1, 2, 1, 2, false, (void*)sFml4.f, false};


    pDWTArg->sFwdHoriKerCfg[0] = sKernelCfg_fwd_hori_p;
    pDWTArg->sFwdHoriKerCfg[1] = sKernelCfg_fwd_hori_u;
    pDWTArg->sBwdHoriKerCfg[0] = sKernelCfg_bwd_hori_u;
    pDWTArg->sBwdHoriKerCfg[1] = sKernelCfg_bwd_hori_p;

    pDWTArg->sFwdVertKerCfg[0] = sKernelCfg_fwd_vert_p;
    pDWTArg->sFwdVertKerCfg[1] = sKernelCfg_fwd_vert_u;
    pDWTArg->sBwdVertKerCfg[0] = sKernelCfg_bwd_vert_u;
    pDWTArg->sBwdVertKerCfg[1] = sKernelCfg_bwd_vert_p;
}

// TODO: panelId
void dwt_row_decomposition(Img_t* pInImg, const DWTArg_t* pArg, const int widthTmp){
    ROI_t sInImgROI = {0, 0, 0, widthTmp, pInImg->height};
    for (int n = 0; n < pArg->numLiftingSteps; ++n){
        ROI_t sOutImgROI = {0, 0, (n+1)%2, widthTmp-((n+1)%2), pInImg->height}; 
        sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI, pArg->sFwdHoriKerCfg[n]);
    }
    dwt_horizontal_reorder(pInImg, sInImgROI);
}

// TODO: panelId
void dwt_column_decomposition(Img_t* pInImg, const DWTArg_t* pArg, const int heightTmp){
    ROI_t sInImgROI = {0, 0, 0, pInImg->width, heightTmp};
    for (int n = 0; n < pArg->numLiftingSteps; ++n){
        ROI_t sOutImgROI = {0, (n+1)%2, 0, pInImg->width, heightTmp-((n+1)%2)}; 
        sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI, pArg->sFwdVertKerCfg[n]);
    }
    dwt_vertical_reorder(pInImg, sInImgROI);
}

typedef void (*FDECOMP)(Img_t*, const DWTArg_t*, const int);

IMG_RTN_CODE dwt_forward_1d(Img_t* pInImg, void* pDWTArg){
    DWTArg_t* pArg = (DWTArg_t*)pDWTArg;
    int widthTmp = pInImg->width;
    int heightTmp = pInImg->height;
    FDECOMP f_hori = NULL;
    FDECOMP f_vert = NULL;

    if (pArg->dim == HORIZONTAL){
        f_hori = dwt_row_decomposition;
    }
    else if (pArg->dim == VERTICAL){
        f_vert = dwt_column_decomposition;
    }
    else if (pArg->dim == TWO_DIMENSIONAL){
        f_hori = dwt_row_decomposition;
        f_vert = dwt_column_decomposition;
    }

    for (int lv = 1; lv <= pArg->level; ++lv){
        if (f_hori != NULL){
            f_hori(pInImg, pArg, widthTmp);
        }
        if (f_vert != NULL){
            f_vert(pInImg, pArg, heightTmp);
        }
        widthTmp = (widthTmp + 1) >> 1;
        heightTmp = (heightTmp + 1) >> 1;
    }
    return SUCCEED;
}

IMG_RTN_CODE dwt_backward_1d(Img_t* pInImg, void* pDWTArg){
    DWTArg_t* pArg = (DWTArg_t*)pDWTArg;
    int* pWidthAll = (int*)malloc(sizeof(int) * pArg->level + 1);
    pWidthAll[0] = pInImg->width;
    for (int l = 1; l < pArg->level; ++l){
        pWidthAll[l] = (pWidthAll[l-1] + 1) >> 1;
    }
    for (int lv = pArg->level; lv >= 1; --lv){
        ROI_t sInImgROI = {0, 0, 0, pWidthAll[lv-1], pInImg->height};
        dwt_horizontal_reorder_back(pInImg, sInImgROI);
        for (int n = 0; n < pArg->numLiftingSteps; ++n){
            ROI_t sOutImgROI = {0, 0, n%2, pWidthAll[lv-1]-(n%2), pInImg->height}; 
            sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI, pArg->sBwdHoriKerCfg[n]);
        }
    }
    free(pWidthAll);
    return SUCCEED;
}

void test_dwt_forward_1d(){
    DWTArg_t* pDWTArg = (DWTArg_t*)malloc(sizeof(DWTArg_t));
    config_kernels_horizontal_LeGall53<int>(pDWTArg, TWO_DIMENSIONAL, 2, MIRROR);


    Img_t* pInImg =(Img_t*)malloc(sizeof(Img_t));
    IMAGE_FMT imageFormat = MONO; // out can be different than in
    size_t width = 10; // out can be different than in
    size_t height = 10; // out can be different than in
    SIGN sign = SIGNED; // out can be different than in
    size_t bitDepth = 32; // out and kernel must be the same as in, and you should be careful about the sign,
    // i.e., if out img is signed but in img is unsigned, since the in img data type will be treated as out img data type,
    // the large unsigned values (from "in img") will be interpreted into negative signed values; to prevent this, you
    // may choose a "larger" data type for in img.
    size_t alignment = 32; // out must be the same as in
    bool allocateImage = true; // recommend that out = in = true

    construct_img(pInImg, 
                  imageFormat,
                  width,
                  height,
                  sign,
                  bitDepth,
                  alignment,
                  allocateImage);
    
    ValCfg_t sValCfg = {pInImg->sign, rand_num_uniform, {0, 49, 0, 0}};

    set_value(pInImg, sValCfg);


    dwt_forward_1d(pInImg, (void*)pDWTArg);


    ROI_t viewROI = {0, 0, 0, pInImg->width, pInImg->height};
    std::cout<<"after dwt:\n";
    view_image_data(pInImg, viewROI);

    // dwt_backward_1d(pInImg, (void*)pDWTArg);

    // std::cout<<"after idwt:\n";
    // view_image_data(pInImg, viewROI);
}

void test_dwt(){
 
    //std::srand(std::time(NULL)); // only need once (maybe for each distribution). how to encapsulate?

    Img_t* pImg1 =(Img_t*)malloc(sizeof(Img_t));
    IMAGE_FMT imageFormat = MONO; // out can be different than in
    size_t width = 10; // out can be different than in
    size_t height = 8; // out can be different than in
    SIGN sign = SIGNED; // out can be different than in
    size_t bitDepth = 32; // out and kernel must be the same as in, and you should be careful about the sign,
    // i.e., if out img is signed but in img is unsigned, since the in img data type will be treated as out img data type,
    // the large unsigned values (from "in img") will be interpreted into negative signed values; to prevent this, you
    // may choose a "larger" data type for in img.
    size_t alignment = 32; // out must be the same as in
    bool allocateImage = true; // recommend that out = in = true

    construct_img(pImg1, 
                  imageFormat,
                  width,
                  height,
                  sign,
                  bitDepth,
                  alignment,
                  allocateImage);
    
    ValCfg_t sValCfg = {pImg1->sign, rand_num_uniform, {0, 49, 0, 0}};

    set_value(pImg1, sValCfg);
    view_img_properties(pImg1);

    std::cout<<"original:\n";
    ROI_t viewROI_1 = {0,0,0,width,height};
    view_image_data(pImg1, viewROI_1);

    Img_t* pImg2 =(Img_t*)malloc(sizeof(Img_t));
    construct_img(pImg2, 
                  imageFormat,
                  width,
                  height,
                  sign,
                  bitDepth,
                  alignment,
                  allocateImage);

    Formulas_T<int> pMyFml;
    pMyFml.f = LeGall53_fwd_predict;

    const KernelCfg_t sKernelCfg_fwd_p = {
        NULL, 1, 3, 0, 0, MIRROR, 2, 1, 2, 1, false, (void*)pMyFml.f, false};
    
    ROI_t sInImgROI = {0, 0, 0, width, height};
    ROI_t sOutImgROI = {0, 0, 1, width-1, height}; 
    
    sliding_window(pImg1, sInImgROI, pImg1, sOutImgROI, sKernelCfg_fwd_p);
    std::cout<<"filtered1:\n";
    ROI_t viewROI = {0,0,0,width,height};
    view_image_data(pImg1, viewROI);
 

    Formulas_T<int> MyFml2;
    MyFml2.f = LeGall53_fwd_update;
    const KernelCfg_t sKernelCfg_fwd_u = {
    NULL, 1, 3, 1, 0, MIRROR, 2, 1, 2, 1, false, (void*)MyFml2.f, false};
    ROI_t sOutImgROI2 = {0, 0, 0, width, height}; 
    sliding_window(pImg1, sInImgROI, pImg1, sOutImgROI2, sKernelCfg_fwd_u);

    std::cout<<"filtered2:\n";
    ROI_t viewROI2 = {0,0,0,width,height};
    view_image_data(pImg1, viewROI2);

    // level 2
    dwt_horizontal_reorder(pImg1, sOutImgROI2);
    std::cout<<"re-ordered:\n";
    view_image_data(pImg1, viewROI2);

    ROI_t sInImgROI_Lv2 = {0, 0, 0, width/2, height};
    ROI_t sOutImgROI_lv2 = {0, 0, 1, width/2-1, height}; 
    sliding_window(pImg1, sInImgROI_Lv2, pImg1, sOutImgROI_lv2, sKernelCfg_fwd_p);
    std::cout<<"filtered_predict_lv2:\n";
    view_image_data(pImg1, viewROI2);

    ROI_t sOutImgROI2_lv2 = {0, 0, 0, width/2, height}; 
    sliding_window(pImg1, sInImgROI_Lv2, pImg1, sOutImgROI2_lv2, sKernelCfg_fwd_u);
    std::cout<<"filtered_update_lv2:\n";
    view_image_data(pImg1, viewROI2);

    //==================================

    Formulas_T<int> MyFml3;
    MyFml3.f = LeGall53_bwd_update;
    const KernelCfg_t sKernelCfg_bwd_u = {
    NULL, 1, 3, 1, 0, MIRROR, 2, 1, 2, 1, false, (void*)MyFml3.f, false};

    sliding_window(pImg1, sInImgROI_Lv2, pImg1, sOutImgROI2_lv2, sKernelCfg_bwd_u);
    std::cout<<"filtered_bwd_update_lv2:\n";
    view_image_data(pImg1, viewROI2);


    Formulas_T<int> MyFml4;
    MyFml4.f = LeGall53_bwd_predict;
    const KernelCfg_t sKernelCfg_bwd_p = {
    NULL, 1, 3, 0, 0, MIRROR, 2, 1, 2, 1, false, (void*)MyFml4.f, false};

    sliding_window(pImg1, sInImgROI_Lv2, pImg1, sOutImgROI_lv2, sKernelCfg_bwd_p);
    std::cout<<"filtered_bwd_predict_lv2:\n";
    view_image_data(pImg1, viewROI2);
    //====================

    dwt_horizontal_reorder_back(pImg1, sOutImgROI2);
    std::cout<<"reorder after lv2 inv dwt:\n";
    view_image_data(pImg1, viewROI2);

    sliding_window(pImg1, sInImgROI, pImg1, sOutImgROI2, sKernelCfg_bwd_u);
    std::cout<<"filtered3:\n";
    view_image_data(pImg1, viewROI2);


    sliding_window(pImg1, sInImgROI, pImg1, sOutImgROI, sKernelCfg_bwd_p);

    std::cout<<"filtered4:\n";
    view_image_data(pImg1, viewROI2);

    destruct_img(&pImg1);
    destruct_img(&pImg2);
}