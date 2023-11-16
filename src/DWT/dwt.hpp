#ifndef _DWT_H
#define _DWT_H

#include "../Infra/ImgDef.hpp"
#include "../Filter/SlidingWindow.hpp"

#define MAX_NUM_STEPS 8


typedef struct{
    int level;
    int numLiftingSteps;
    KernelCfg_t sFwdDwtKerCfg[MAX_NUM_STEPS];
    KernelCfg_t sBwdDwtKerCfg[MAX_NUM_STEPS];
} DWTArg_t;


void test_dwt();
void test_dwt_forward_1d();
void test_config_kernel();

IMG_RTN_CODE dwt_forward_1d(Img_t* pInImg, void* pAlgoArg);
IMG_RTN_CODE dwt_backward_1d(Img_t* pInImg, void* pDWTArg);

#endif