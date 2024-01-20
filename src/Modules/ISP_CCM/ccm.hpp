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
    ColorMatRow1_t sColorMatRow1;
    ColorMatRow2_t sColorMatRow2;
    ColorMatRow3_t sColorMatRow3;
} CCMArg_t;

typedef struct{
    CCMArg_t sCCMArg;
} MArg_CCM_t;

Hash_t default_ccm_arg_hash();
const MArg_CCM_t get_ccm_arg_struct_from_hash(Hash_t* pHs);
IMG_RTN_CODE isp_ccm(const ImgPtrs_t sImgPtrs, Img_t* pOutImg, Hash_t* pMArg_CCM, bool updateArgs);

void test_ccm_hash();

#endif