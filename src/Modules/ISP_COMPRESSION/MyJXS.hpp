#ifndef _MY_JXS_H
#define _MY_JXS_H

//#include "../Infra/ImgDef.hpp"
#include "../../Algos/StarTetrix/StarTetrixTrans.hpp"
#include "../../Algos/DWT/dwt.hpp"

typedef struct{
    StarTetrixArg_t sStarTetrixArg;
    DWTArg_t sDWTArg;
} MyJXSArg_t;

IMG_RTN_CODE my_jxs_forward(const Img_t* pInImg, Img_t* pOutImg, const void* pMyJXSArg);
IMG_RTN_CODE my_jxs_forward(const Img_t* pInImg, Img_t* pOutImg, const void* pMyJXSArg);
IMG_RTN_CODE my_jxs_pipe_sim(const Img_t* pInImg, Img_t* pOutImg, const void* pMyJXSArg); // this is forward + backward

void test_my_jxs();

#endif