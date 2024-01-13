#ifndef _CCM_H
#define _CCM_H

#include "../COMMON/common.hpp"
#include "../../Algos/Color/Color.hpp"

typedef struct{
    int RR;
    int RG;
    int RB;
} ColorMatRow1_t;

typedef struct{
    int GR;
    int GG;
    int GB;
} ColorMatRow2_t;

typedef struct{
    int BR;
    int BG;
    int BB;
} ColorMatRow3_t;

typedef struct{
    ColorMatRow1_t colorMatRow1;
    ColorMatRow2_t colorMatRow2;
    ColorMatRow3_t colorMatRow3;
} CCMArg_t;

typedef struct{
    CCMArg_t sCCMArg;
} MArg_CCM_t;

Hash_t default_ccm_arg_hash();
IMG_RTN_CODE isp_ccm(const ImgPtrs_t sImgPtrs, Img_t* pOutImg, void* pMArg_CCM);

void test_ccm_hash();

#endif