#ifndef _DWT_H
#define _DWT_H

#include "../Infra/ImgDef.hpp"
#include "../Filter/SlidingWindow.hpp"
#include "../Math/Math.hpp"

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
void demo_dwt();

IMG_RTN_CODE dwt_forward(Img_t* pInImg, Img_t* pOutImg, void* pAlgoArg);
IMG_RTN_CODE dwt_backward(Img_t* pInImg, Img_t* pOutImg, void* pDWTArg);


template<typename T>
const T LeGall53_fwd_predict(const T** a, const T** b = NULL, const int vecLen = 3){
    return **(a+1) - ((**a + **(a+2))>>1);
}

template<typename T>
const T LeGall53_fwd_update(const T** a, const T** b = NULL, const int vecLen = 3){
    return **(a+1) + ((**a + **(a+2))>>2);
}

template<typename T>
const T LeGall53_bwd_predict(const T** a, const T** b = NULL, const int vecLen = 3){
    return **(a+1) + ((**a + **(a+2))>>1);
}

template<typename T>
const T LeGall53_bwd_update(const T** a, const T** b = NULL, const int vecLen = 3){
    return **(a+1) - ((**a + **(a+2))>>2);
}

template<typename T>
void config_dwt_kernels_LeGall53(DWTArg_t* pDWTArg, const PADDING& padding){
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


#endif