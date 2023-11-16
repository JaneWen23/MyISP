#ifndef _DWT_H
#define _DWT_H

#include "../Infra/ImgDef.hpp"
#include "../Filter/SlidingWindow.hpp"

#define MAX_NUM_STEPS 8


typedef struct{
    int level;
    int numLiftingSteps;
    KernelCfg_t* pDwtKerCfg[MAX_NUM_STEPS]; // array of reference is not allowed, must use array of pointers
} DWTArg_t;


void test_dwt();
void test_dwt_forward_1d();

IMG_RTN_CODE dwt_forward_1d(Img_t* pInImg, void* pAlgoArg);
IMG_RTN_CODE dwt_backward_1d(Img_t* pInImg, void* pDWTArg);

#endif