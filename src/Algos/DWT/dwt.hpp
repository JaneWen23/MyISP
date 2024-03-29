#ifndef _DWT_H
#define _DWT_H

#include "../Infra/ImgDef.hpp"
#include "../Filter/SlidingWindow.hpp"
#include "../Math/Math.hpp"

#define MAX_NUM_STEPS 4 // forward or backward max number of lifting steps

typedef enum{
    HORIZONTAL,
    VERTICAL,
    TWO_DIMENSIONAL
} ORIENT;

typedef enum{
    LE_GALL_53
    // and so on ...
} WAVELET_NAME;

typedef struct{
    int inImgPanelId; // apply dwt to the whole 2D image???
    int outImgPanelId;
    ORIENT orient;
    int level;
    WAVELET_NAME wavelet;
    PADDING padding;
} DWTArg_t;


void test_dwt();

IMG_RTN_CODE dwt_forward(const Img_t* pInImg, Img_t* pOutImg, const void* pDWTArg);
IMG_RTN_CODE dwt_backward(const Img_t* pInImg, Img_t* pOutImg, const void* pDWTArg);
const char* get_dwt_orient_name(const ORIENT orient);
const ORIENT get_dwt_orient_from_name(std::string name);
const char* get_wavelet_name(const WAVELET_NAME wavelet);
const WAVELET_NAME get_wavelet_from_name(std::string name);

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




#endif