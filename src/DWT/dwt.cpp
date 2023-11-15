#include <stdlib.h>
#include <iostream>
#include "../Filter/SlidingWindow.hpp"
#include "../Math/Math.hpp"
#include "dwt.hpp"

#include "../Infra/RandImageGen.hpp"




IMG_RTN_CODE dwt_forward_1d(Img_t* pInImg, Img_t* pOutImg, void* pDWTArg){
    // sliding_window(Img_t* pInImg, const ROI_t& sInImgROI, Img_t* pOutImg, const ROI_t& sOutImgROI, const KernelCfg_t& sKernelCfg)
    return SUCCEED;
}

template<typename T>
void dwt_horizontal_reorder(Img_t* pImg, const ROI_t sImgROI){
    // ImgROI.startCol must be 0, otherwise, there will be errors.
    T tmp = 0;
    int strideInPix = pImg->strides[sImgROI.panelId] / sizeof(T);
    for (int i = sImgROI.startRow; i < sImgROI.startRow + sImgROI.roiHeight; ++i){
        for (int t = 1; t < sImgROI.roiWidth>>1; ++t){
            for (int j = t; j < sImgROI.roiWidth - t; j += 2){
                tmp = *((T*)pImg->pImageData[sImgROI.panelId] + i*strideInPix + j);
                *((T*)pImg->pImageData[sImgROI.panelId] + i*strideInPix + j) = *((T*)pImg->pImageData[sImgROI.panelId] + i*strideInPix+ j + 1);
                *((T*)pImg->pImageData[sImgROI.panelId] + i*strideInPix + j + 1) = tmp;
            }
        }
    }
}

template<typename T>
void dwt_horizontal_reorder_back(Img_t* pImg, const ROI_t sImgROI){
    // ImgROI.startCol must be 0, otherwise, there will be errors.
    T tmp = 0;
    int strideInPix = pImg->strides[sImgROI.panelId] / sizeof(T);
    for (int i = sImgROI.startRow; i < sImgROI.startRow + sImgROI.roiHeight; ++i){
        for (int t = (sImgROI.roiWidth-1)>>1; t > 0; --t){
            for (int j = t; j < sImgROI.roiWidth - t; j += 2){
                tmp = *((T*)pImg->pImageData[sImgROI.panelId] + i*strideInPix + j);
                *((T*)pImg->pImageData[sImgROI.panelId] + i*strideInPix + j) = *((T*)pImg->pImageData[sImgROI.panelId] + i*strideInPix+ j + 1);
                *((T*)pImg->pImageData[sImgROI.panelId] + i*strideInPix + j + 1) = tmp;
            }
        }
    }
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
    // std::cout<<"filtered1:\n";
    // ROI_t viewROI = {0,0,0,width,height};
    // view_image_data(pImg1, viewROI);
 

    Formulas_T<int> pMyFml2;
    pMyFml2.f = LeGall53_fwd_update;
    const KernelCfg_t sKernelCfg_fwd_u = {
    NULL, 1, 3, 1, 0, MIRROR, 2, 1, 2, 1, false, (void*)pMyFml2.f, false};
    ROI_t sOutImgROI2 = {0, 0, 0, width, height}; 
    sliding_window(pImg1, sInImgROI, pImg1, sOutImgROI2, sKernelCfg_fwd_u);

    // std::cout<<"filtered2:\n";
     ROI_t viewROI2 = {0,0,0,width,height};
    // view_image_data(pImg1, viewROI2);

    // level 2
    dwt_horizontal_reorder<int>(pImg1, sOutImgROI2);
    std::cout<<"re-ordered:\n";
    view_image_data(pImg1, viewROI2);

    ROI_t sInImgROI_Lv2 = {0, 0, 0, width/2-1, height}; // !!!
    ROI_t sOutImgROI_lv2 = {0, 0, 1, width/2-1, height}; 
    sliding_window(pImg1, sInImgROI_Lv2, pImg1, sOutImgROI_lv2, sKernelCfg_fwd_p);
    std::cout<<"filtered_predict_lv2:\n";
    view_image_data(pImg1, viewROI2);

    ROI_t sOutImgROI2_lv2 = {0, 0, 0, width/2, height}; 
    sliding_window(pImg1, sInImgROI_Lv2, pImg1, sOutImgROI2_lv2, sKernelCfg_fwd_u);
    std::cout<<"filtered_update_lv2:\n";
    view_image_data(pImg1, viewROI2);

    //==================================

    Formulas_T<int> pMyFml3;
    pMyFml3.f = LeGall53_bwd_update;
    const KernelCfg_t sKernelCfg_bwd_u = {
    NULL, 1, 3, 1, 0, MIRROR, 2, 1, 2, 1, false, (void*)pMyFml3.f, false};

    sliding_window(pImg1, sInImgROI_Lv2, pImg1, sOutImgROI2_lv2, sKernelCfg_bwd_u);
    std::cout<<"filtered_bwd_update_lv2:\n";
    view_image_data(pImg1, viewROI2);


    Formulas_T<int> pMyFml4;
    pMyFml4.f = LeGall53_bwd_predict;
    const KernelCfg_t sKernelCfg_bwd_p = {
    NULL, 1, 3, 0, 0, MIRROR, 2, 1, 2, 1, false, (void*)pMyFml4.f, false};

    sliding_window(pImg1, sInImgROI_Lv2, pImg1, sOutImgROI_lv2, sKernelCfg_bwd_p);
    std::cout<<"filtered_bwd_predict_lv2:\n";
    view_image_data(pImg1, viewROI2);
    //====================

    dwt_horizontal_reorder_back<int>(pImg1, sOutImgROI2);
    std::cout<<"reorder after lv2 inv dwt:\n";
    view_image_data(pImg1, viewROI2);

    sliding_window(pImg1, sInImgROI, pImg1, sOutImgROI2, sKernelCfg_bwd_u);
    std::cout<<"filtered3:\n";
    ROI_t viewROI3 = {0,0,0,width,height};
    view_image_data(pImg1, viewROI2);


    // Formulas_T<int> pMyFml4;
    // pMyFml4.f = LeGall53_bwd_predict;
    // const KernelCfg_t sKernelCfg4 = {
    // NULL, 1, 3, 0, 0, MIRROR, 2, 1, 2, 1, false, (void*)pMyFml4.f, false};
    // ROI_t sOutImgROI4 = {0, 0, 1, width-1, height}; 
    sliding_window(pImg1, sInImgROI, pImg1, sOutImgROI, sKernelCfg_bwd_p);

    std::cout<<"filtered4:\n";
    ROI_t viewROI4 = {0,0,0,width,height};
    view_image_data(pImg1, viewROI2);

    destruct_img(&pImg1);
    destruct_img(&pImg2);
}