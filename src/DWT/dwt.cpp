#include <stdlib.h>
#include <iostream>
#include "../Filter/SlidingWindow.hpp"
#include "../Infra/RandImageGen.hpp"
#include "dwt.hpp"


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

void dwt_row_analysis(Img_t* pInImg, const DWTArg_t* pArg, const int widthTmp, const int HeightTmp){
    ROI_t sInImgROI = {pArg->inImgPanelId, 0, 0, widthTmp, HeightTmp};
    for (int n = 0; n < pArg->numLiftingSteps; ++n){
        ROI_t sOutImgROI = {pArg->outImgPanelId, 0, (n+1)%2, widthTmp-((n+1)%2), HeightTmp}; 
        sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI, pArg->sFwdHoriKerCfg[n]);
    }
    dwt_horizontal_reorder(pInImg, sInImgROI);
}

void dwt_column_analysis(Img_t* pInImg, const DWTArg_t* pArg, const int widthTmp, const int heightTmp){
    ROI_t sInImgROI = {pArg->inImgPanelId, 0, 0, widthTmp, heightTmp};
    for (int n = 0; n < pArg->numLiftingSteps; ++n){
        ROI_t sOutImgROI = {pArg->outImgPanelId, (n+1)%2, 0, widthTmp, heightTmp-((n+1)%2)}; 
        sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI, pArg->sFwdVertKerCfg[n]);
    }
    dwt_vertical_reorder(pInImg, sInImgROI);
}

void dwt_row_synthesis(Img_t* pInImg, const DWTArg_t* pArg, const int widthTmp, const int heightTmp){
    ROI_t sInImgROI = {pArg->inImgPanelId, 0, 0, widthTmp, heightTmp};
    dwt_horizontal_reorder_back(pInImg, sInImgROI);
    for (int n = 0; n < pArg->numLiftingSteps; ++n){
        ROI_t sOutImgROI = {pArg->outImgPanelId, 0, n%2, widthTmp-(n%2), heightTmp}; 
        sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI, pArg->sBwdHoriKerCfg[n]);
    }
}

void dwt_column_synthesis(Img_t* pInImg, const DWTArg_t* pArg, const int widthTmp, const int heightTmp){
    ROI_t sInImgROI = {pArg->inImgPanelId, 0, 0, widthTmp, heightTmp};
    dwt_vertical_reorder_back(pInImg, sInImgROI);
    for (int n = 0; n < pArg->numLiftingSteps; ++n){
        ROI_t sOutImgROI = {pArg->outImgPanelId, n%2, 0, widthTmp, heightTmp-(n%2)}; 
        sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI, pArg->sBwdVertKerCfg[n]);
    }
}

typedef void (*FAS)(Img_t*, const DWTArg_t*, const int, const int);

IMG_RTN_CODE dwt_forward(Img_t* pInImg, Img_t* pOutImg, void* pDWTArg){
    DWTArg_t* pArg = (DWTArg_t*)pDWTArg;

    duplicate_img(pInImg, pOutImg);

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
            f_hori(pOutImg, pArg, widthTmp, heightTmp);
        }
        if (f_vert != NULL){
            f_vert(pOutImg, pArg, widthTmp, heightTmp);
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


IMG_RTN_CODE dwt_backward(Img_t* pInImg, Img_t* pOutImg, void* pDWTArg){
    DWTArg_t* pArg = (DWTArg_t*)pDWTArg;
    
    duplicate_img(pInImg, pOutImg);
    
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
            f_vert(pOutImg, pArg, pWidthAll[lv-1], pHeightAll[lv-1]); // NOTE: the order is reverse of fwd transform.
        }
        if (f_hori != NULL){
            f_hori(pOutImg, pArg, pWidthAll[lv-1], pHeightAll[lv-1]);
        }
    }
    free(pWidthAll);
    free(pHeightAll);
    return SUCCEED;
}


void test_dwt(){
 
    //std::srand(std::time(NULL)); // only need once (maybe for each distribution). how to encapsulate?

    DWTArg_t* pDWTArg = (DWTArg_t*)malloc(sizeof(DWTArg_t));
    pDWTArg->level = 2;
    pDWTArg->orient = TWO_DIMENSIONAL;
    pDWTArg->inImgPanelId = 0;
    pDWTArg->outImgPanelId = 0;
    config_dwt_kernels_LeGall53<int>(pDWTArg, MIRROR);


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

    std::cout<<"original:\n";
    ROI_t viewROI = {0, 0, 0, pInImg->width, pInImg->height};
    view_image_data(pInImg, viewROI );

    Img_t* pOutImg = (Img_t*)malloc(sizeof(Img_t));

    assert(pDWTArg->level > 0);
    dwt_forward(pInImg, pOutImg, (void*)pDWTArg);

    
    std::cout<<"after dwt:\n";
    view_image_data(pOutImg, viewROI);

    Img_t* pOutBackImg = (Img_t*)malloc(sizeof(Img_t));

    dwt_backward(pOutImg, pOutBackImg, (void*)pDWTArg);

    std::cout<<"after idwt:\n";
    view_image_data(pOutBackImg, viewROI);

    destruct_img(&pInImg);
    destruct_img(&pOutImg);
    destruct_img(&pOutBackImg);
}

