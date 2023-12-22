#ifndef _MY_JXS_H
#define _MY_JXS_H

#include "../COMMON/common.hpp"
#include "../../Algos/StarTetrix/StarTetrixTrans.hpp"
#include "../../Algos/DWT/dwt.hpp"


typedef struct{
    StarTetrixArg_t sStarTetrixArg;
    DWTArg_t sDWTArg;
    // more...
} MArg_Compression_t;


IMG_RTN_CODE isp_compression(const ImgPtrs_t sImgPtrs, Img_t* pOutImg, void* pMyJXSArg); // this is forward + backward

void test_my_jxs();

#endif