#ifndef _STAR_TETRIX_TRANS_H
#define _STAR_TETRIX_TRANS_H

#include "../Infra/ImgDef.hpp"
#include "../Filter/SlidingWindow.hpp"

typedef struct{
    int Wr;
    int Wb;
} StarTetrixArg_t;

IMG_RTN_CODE star_tetrix_forward(const Img_t* pInImg, Img_t* pOutImg, const void* pStarTetrixArg);
IMG_RTN_CODE star_tetrix_backward(const Img_t* pInImg, Img_t* pOutImg, const void* pStarTetrixArg);

void test_star_tetrix();



template<typename T>
const T StarTetrix_fwd_CbCr(const T** a, const T** b = NULL, const int vecLen = 9){
    // "**a" is the start of the 3x3 grids.
    return **(a+4) - ((**(a+1) + **(a+3) + **(a+5) + **(a+7)) >> 2);
}

template<typename T>
const T StarTetrix_fwd_Y1Y2(const T** a, const T** b, const int vecLen = 9){
    // "**a" is the start of the 3x3 grids. "**b" is the start of white balance coefficients.
    return **(a+4) + ( ((**(a+1) + **(a+7)) * (**b) + (**(a+3) + **(a+5)) * (**(b+1))) >> 3);
}

template<typename T>
const T StarTetrix_fwd_Delta(const T** a, const T** b = NULL, const int vecLen = 9){
    // "**a" is the start of the 3x3 grids.
    return **(a+4) - ((**a + **(a+2) + **(a+6) + **(a+8))>>2);
}

template<typename T>
const T StarTetrix_fwd_Ybar(const T** a, const T** b = NULL, const int vecLen = 9){
    // "**a" is the start of the 3x3 grids.
    return **(a+4) + ((**a + **(a+2) + **(a+6) + **(a+8))>>3);
}

template<typename T>
const T StarTetrix_bwd_BR(const T** a, const T** b = NULL, const int vecLen = 9){
    // "**a" is the start of the 3x3 grids.
    return **(a+4) + ((**(a+1) + **(a+3) + **(a+5) + **(a+7)) >> 2);
}

template<typename T>
const T StarTetrix_bwd_GrGb(const T** a, const T** b, const int vecLen = 9){
    // "**a" is the start of the 3x3 grids. "**b" is the start of white balance coefficients.
    return **(a+4) - ( ((**(a+1) + **(a+7)) * (**b) + (**(a+3) + **(a+5)) * (**(b+1))) >> 3);
}

template<typename T>
const T StarTetrix_bwd_Y1(const T** a, const T** b = NULL, const int vecLen = 9){
    // "**a" is the start of the 3x3 grids.
    return **(a+4) + ((**a + **(a+2) + **(a+6) + **(a+8))>>2);
}

template<typename T>
const T StarTetrix_bwd_Y2(const T** a, const T** b = NULL, const int vecLen = 9){
    // "**a" is the start of the 3x3 grids.
    return **(a+4) - ((**a + **(a+2) + **(a+6) + **(a+8))>>3);
}

#endif