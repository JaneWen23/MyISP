#ifndef _MY_JXS_H
#define _MY_JXS_H

//#include "../Infra/ImgDef.hpp"
#include "../StarTetrix/StarTetrixTrans.hpp"
#include "../DWT/dwt.hpp"

typedef struct{
    StarTetrixArg_t sStarTetrixArg;
    DWTArg_t sDWTArg;
} MyJXSArg_t;

IMG_RTN_CODE my_jxs_forward(const Img_t* pInImg, Img_t* pOutImg, const void* pMyJXSArg);
IMG_RTN_CODE my_jxs_backward(const Img_t* pInImg, Img_t* pOutImg, const void* pMyJXSArg);
void test_my_jxs();

#endif