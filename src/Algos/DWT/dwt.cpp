#include <stdlib.h>
#include <iostream>
#include "../Filter/SlidingWindow.hpp"
#include "../Infra/RandImageGen.hpp"
#include "dwt.hpp"

const char* get_dwt_orient_name(const ORIENT orient){
    switch(orient){
        case(HORIZONTAL):{
            return "HORIZONTAL";
        }
        case(VERTICAL):{
            return "VERTICAL";
        }
        case(TWO_DIMENSIONAL):{
            return "TWO_DIMENSIONAL";
        }
    }
    return "HORIZONTAL";
}

const ORIENT get_dwt_orient_from_name(const char* name){
    if (strcmp(name, "HORIZONTAL") == 0){
        return HORIZONTAL;
    }
    if (strcmp(name, "VERTICAL") == 0){
        return VERTICAL;
    }
    if (strcmp(name, "TWO_DIMENSIONAL") == 0){
        return TWO_DIMENSIONAL;
    }
    return HORIZONTAL; // nonsense
}

const char* get_wavelet_name(const WAVELET_NAME wavelet){
    switch(wavelet){
        case(LE_GALL_53):{
            return "LE_GALL_53";
        }
    }
    return "LE_GALL_53";
}

const WAVELET_NAME get_wavelet_from_name(const char* name){
    if (strcmp(name, "LE_GALL_53") == 0){
        return LE_GALL_53;
    }
    return LE_GALL_53; // nonsense
}

typedef struct{
    int inImgPanelId; // apply dwt to the whole 2D image???
    int outImgPanelId;
    ORIENT orient;
    int level;
    int numLiftingSteps; // defined by a certain wavelet
    KernelCfg_t sFwdHoriKerCfg[MAX_NUM_STEPS]; // defined by a certain wavelet
    KernelCfg_t sBwdHoriKerCfg[MAX_NUM_STEPS]; // defined by a certain wavelet
    KernelCfg_t sFwdVertKerCfg[MAX_NUM_STEPS]; // defined by a certain wavelet
    KernelCfg_t sBwdVertKerCfg[MAX_NUM_STEPS]; // defined by a certain wavelet
} DWTRunTimeArg_t;

template<typename T>
void config_dwt_kernels_LeGall53(DWTRunTimeArg_t* pDWTArg, const PADDING& padding){
    pDWTArg->numLiftingSteps = 2; // defined by LeGall 5/3

    Formulas_T<T> sFml1;
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

template <typename T>
struct DWTKerCfg_T{
    void (*f)(DWTRunTimeArg_t*,  const PADDING&);
};

template <typename T>
void perform_dwt_ker_cfg(WAVELET_NAME wavelet, const PADDING& padding, DWTRunTimeArg_t* pDWTArg){
    DWTKerCfg_T<T> kerCfg;
    switch (wavelet){
        case LE_GALL_53:{
            kerCfg.f = config_dwt_kernels_LeGall53<T>;
        }
        // and more cases ...
        default:{
            kerCfg.f = config_dwt_kernels_LeGall53<T>;
        }
    }
    kerCfg.f(pDWTArg, padding);
}

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

void dwt_row_analysis(Img_t* pInImg, const DWTRunTimeArg_t* pArg, const int widthTmp, const int HeightTmp){
    ROI_t sInImgROI = {pArg->inImgPanelId, 0, 0, widthTmp, HeightTmp};
    for (int n = 0; n < pArg->numLiftingSteps; ++n){
        ROI_t sOutImgROI = {pArg->outImgPanelId, 0, (n+1)%2, widthTmp-((n+1)%2), HeightTmp}; 
        sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI, pArg->sFwdHoriKerCfg[n]);
    }
    dwt_horizontal_reorder(pInImg, sInImgROI);
}

void dwt_column_analysis(Img_t* pInImg, const DWTRunTimeArg_t* pArg, const int widthTmp, const int heightTmp){
    ROI_t sInImgROI = {pArg->inImgPanelId, 0, 0, widthTmp, heightTmp};
    for (int n = 0; n < pArg->numLiftingSteps; ++n){
        ROI_t sOutImgROI = {pArg->outImgPanelId, (n+1)%2, 0, widthTmp, heightTmp-((n+1)%2)}; 
        sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI, pArg->sFwdVertKerCfg[n]);
    }
    dwt_vertical_reorder(pInImg, sInImgROI);
}

void dwt_row_synthesis(Img_t* pInImg, const DWTRunTimeArg_t* pArg, const int widthTmp, const int heightTmp){
    ROI_t sInImgROI = {pArg->inImgPanelId, 0, 0, widthTmp, heightTmp};
    dwt_horizontal_reorder_back(pInImg, sInImgROI);
    for (int n = 0; n < pArg->numLiftingSteps; ++n){
        ROI_t sOutImgROI = {pArg->outImgPanelId, 0, n%2, widthTmp-(n%2), heightTmp}; 
        sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI, pArg->sBwdHoriKerCfg[n]);
    }
}

void dwt_column_synthesis(Img_t* pInImg, const DWTRunTimeArg_t* pArg, const int widthTmp, const int heightTmp){
    ROI_t sInImgROI = {pArg->inImgPanelId, 0, 0, widthTmp, heightTmp};
    dwt_vertical_reorder_back(pInImg, sInImgROI);
    for (int n = 0; n < pArg->numLiftingSteps; ++n){
        ROI_t sOutImgROI = {pArg->outImgPanelId, n%2, 0, widthTmp, heightTmp-(n%2)}; 
        sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI, pArg->sBwdVertKerCfg[n]);
    }
}

typedef void (*FAS)(Img_t*, const DWTRunTimeArg_t*, const int, const int);

IMG_RTN_CODE dwt_forward(const Img_t* pInImg, Img_t* pOutImg, const void* pDWTArg){
    if (pInImg->sign != SIGNED){
        std::cout<< "error: forward DWT needs input image to be signed, but got unsigned. exited.\n";
        exit(1);
    }

    DWTArg_t* pArg = (DWTArg_t*)pDWTArg;
    DWTRunTimeArg_t sRunTimeArg = {};
    sRunTimeArg.level = pArg->level;
    sRunTimeArg.orient = pArg->orient;
    sRunTimeArg.inImgPanelId = pArg->inImgPanelId;
    sRunTimeArg.outImgPanelId = pArg->outImgPanelId;
   
    if (pInImg->bitDepth <= 8){
        perform_dwt_ker_cfg<int8_t>(pArg->wavelet, pArg->padding, &sRunTimeArg);
    }
    else if (pInImg->bitDepth <= 16){
        perform_dwt_ker_cfg<int16_t>(pArg->wavelet, pArg->padding, &sRunTimeArg);
    }
    else if (pInImg->bitDepth <= 32){
        perform_dwt_ker_cfg<int>(pArg->wavelet, pArg->padding, &sRunTimeArg);
    }
    

    if (pInImg != pOutImg){
        // in img and out img are intentionally different images
        duplicate_img(pInImg, pOutImg);
    }

    int widthTmp = pOutImg->width;
    int heightTmp = pOutImg->height;
    FAS f_hori = NULL;
    FAS f_vert = NULL;

    if (pArg->orient == HORIZONTAL){
        f_hori = dwt_row_analysis;
    }
    else if (pArg->orient == VERTICAL){
        f_vert = dwt_column_analysis;
    }
    else if (pArg->orient == TWO_DIMENSIONAL){
        f_hori = dwt_row_analysis;
        f_vert = dwt_column_analysis;
    }

    for (int lv = 1; lv <= pArg->level; ++lv){ // TODO: hori and vert may have different levels!!!
        if (f_hori != NULL){
            f_hori(pOutImg, &sRunTimeArg, widthTmp, heightTmp);
        }
        if (f_vert != NULL){
            f_vert(pOutImg, &sRunTimeArg, widthTmp, heightTmp);
        }
        if (f_hori != NULL){
            widthTmp = (widthTmp + 1) >> 1;
        }
        if (f_vert != NULL){
            heightTmp = (heightTmp + 1) >> 1;
        }
    }
    return SUCCEED;
}


IMG_RTN_CODE dwt_backward(const Img_t* pInImg, Img_t* pOutImg, const void* pDWTArg){
    if (pInImg->sign != SIGNED){
        std::cout<< "error: backward DWT needs input image to be signed, but got unsigned. exited.\n";
        exit(1);
    }

    DWTArg_t* pArg = (DWTArg_t*)pDWTArg;
    DWTRunTimeArg_t sRunTimeArg = {};
    sRunTimeArg.level = pArg->level;
    sRunTimeArg.orient = pArg->orient;
    sRunTimeArg.inImgPanelId = pArg->inImgPanelId;
    sRunTimeArg.outImgPanelId = pArg->outImgPanelId;
    if (pInImg->bitDepth <= 8){
        perform_dwt_ker_cfg<int8_t>(pArg->wavelet, pArg->padding, &sRunTimeArg);
    }
    else if (pInImg->bitDepth <= 16){
        perform_dwt_ker_cfg<int16_t>(pArg->wavelet, pArg->padding, &sRunTimeArg);
    }
    else if (pInImg->bitDepth <= 32){
        perform_dwt_ker_cfg<int>(pArg->wavelet, pArg->padding, &sRunTimeArg);
    }
    
    
    if (pInImg != pOutImg){
        duplicate_img(pInImg, pOutImg);
    }
    
    int* pWidthAll = (int*)malloc(sizeof(int) * pArg->level + 1);
    int* pHeightAll = (int*)malloc(sizeof(int) * pArg->level + 1);// TODO: hori and vert may have different levels!!!
    pWidthAll[0] = pOutImg->width;
    pHeightAll[0] = pOutImg->height;
    for (int l = 1; l < pArg->level; ++l){
        pWidthAll[l] = (pWidthAll[l-1] + 1) >> 1;
        pHeightAll[l] = (pHeightAll[l-1] + 1) >> 1;
    }

    FAS f_hori = NULL;
    FAS f_vert = NULL;

    if (pArg->orient == HORIZONTAL){
        f_hori = dwt_row_synthesis;
    }
    else if (pArg->orient == VERTICAL){
        f_vert = dwt_column_synthesis;
    }
    else if (pArg->orient == TWO_DIMENSIONAL){
        f_hori = dwt_row_synthesis;
        f_vert = dwt_column_synthesis;
    }

    for (int lv = pArg->level; lv >= 1; --lv){
        if (f_vert != NULL){
            f_vert(pOutImg, &sRunTimeArg, pWidthAll[lv-1], pHeightAll[lv-1]); // NOTE: the order is reverse of fwd transform.
        }
        if (f_hori != NULL){
            f_hori(pOutImg, &sRunTimeArg, pWidthAll[lv-1], pHeightAll[lv-1]);
        }
    }
    free(pWidthAll);
    free(pHeightAll);
    return SUCCEED;
}


void test_dwt(){
 
    //std::srand(std::time(NULL)); // only need once (maybe for each distribution). how to encapsulate?

    DWTArg_t sDWTArg = {};
    sDWTArg.level = 2;
    sDWTArg.orient = TWO_DIMENSIONAL;
    sDWTArg.inImgPanelId = 0;
    sDWTArg.outImgPanelId = 0;
    sDWTArg.wavelet = LE_GALL_53;
    sDWTArg.padding = MIRROR;

    Img_t* pInImg =(Img_t*)malloc(sizeof(Img_t));
    IMAGE_FMT imageFormat = MONO; // out can be different than in
    size_t width = 10; // out can be different than in
    size_t height = 10; // out can be different than in
    SIGN sign = SIGNED; // out can be different than in
    size_t bitDepth = 16; // out and kernel must be the same as in, and you should be careful about the sign,
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

    std::cout<<"original:\n";
    ROI_t viewROI = {0, 0, 0, pInImg->width, pInImg->height};
    view_image_data(pInImg, viewROI );

    Img_t* pOutImg = (Img_t*)malloc(sizeof(Img_t));

    assert(sDWTArg.level > 0);
    dwt_forward(pInImg, pOutImg, (void*)&sDWTArg);

    
    std::cout<<"after dwt:\n";
    view_image_data(pOutImg, viewROI);

    Img_t* pOutBackImg = (Img_t*)malloc(sizeof(Img_t));

    dwt_backward(pOutImg, pOutBackImg, (void*)&sDWTArg);

    std::cout<<"after idwt:\n";
    view_image_data(pOutBackImg, viewROI);

    destruct_img(&pInImg);
    destruct_img(&pOutImg);
    destruct_img(&pOutBackImg);
}

