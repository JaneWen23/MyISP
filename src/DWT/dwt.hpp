#ifndef _DWT_H
#define _DWT_H

#include "../Infra/ImgDef.hpp"
#include "../Filter/SlidingWindow.hpp"

#define MAX_NUM_STEPS 4

typedef enum{
    HORIZONTAL,
    VERTICAL,
    TWO_DIMENSIONAL
} ORIENT;

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
} DWTArg_t;


void test_dwt();
void test_dwt_forward_1d();
void test_config_kernel();

IMG_RTN_CODE dwt_forward(Img_t* pInImg, void* pAlgoArg);
IMG_RTN_CODE dwt_backward(Img_t* pInImg, void* pDWTArg);

#endif