#ifndef _SLIDING_WINDOW_H
#define _SLIDING_WINDOW_H

#include <iostream>
#include <stdlib.h>
#include "../Infra/ImgDef.hpp"
#include "../Infra/RandImageGen.hpp"

typedef enum{
    ZEROPADDING,
    PERIODIC,
    MIRROR
} PADDING;

typedef struct{
    uint8_t* pKernel; // pointer to kernel data; may convert to any type when used; can be NULL if no explicit kernel.
    int kerHeight; // if no explicit kernel, this is equivalently window height, minimum is 1
    int kerWidth; // if no explicit kernel, this is equivalently window width, minimum is 1
    int horiCenter; // horizontal center
    int vertCenter; // vertical center
    PADDING padding; // padding scheme
    int horiStep; // horizontal step (same effect as filtering followed by down-sampling horizontally)
    int vertStep; // vertical step (same effect as filtering followed by down-sampling vertically)
    int horiUpsample; // (down-sampled (or not)) filtering followed by up-sampling horizontally
    int vertUpsample; // (down-sampled (or not)) filtering followed by up-sampling vertically
    bool needFlip; // if true, it is convolution; if false, it is correlation.
    void* formula; // dot product for convolution; self-defined formula for other filtering.
    bool divideAndConquer; // if true, 2d filtering can be defined by summation of several 1d results; if false, 2d filtering will be performed in naive way.
} KernelCfg_t;


IMG_RTN_CODE sliding_window(const Img_t* pInImg, const ROI_t& sInImgROI, Img_t* pOutImg, const ROI_t& sOutImgROI, const KernelCfg_t& sKernelCfg);
IMG_RTN_CODE sliding_window_1x1(const Img_t* pInImg, const ROI_t& sInImgROI, Img_t* pOutImg, const ROI_t& sOutImgROI, const KernelCfg_t& sKernelCfg);
IMG_RTN_CODE image_sampling(const Img_t* pInImg, const ROI_t& sInImgROI, Img_t* pOutImg, const ROI_t& sOutImgROI, const KernelCfg_t& sKernelCfg);
void test_sliding_window();
void test_sliding_window_1x1();
void test_sliding_window_1x1_2();

#endif