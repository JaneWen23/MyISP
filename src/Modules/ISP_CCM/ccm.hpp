#ifndef _CCM_H
#define _CCM_H

#include "../COMMON/common.hpp"
#include "../../Algos/Color/Color.hpp"

typedef struct{
    CCMArg_t sCCMArg;
} MArg_CCM_t;

Hash_t default_ccm_arg_hash();
IMG_RTN_CODE isp_ccm(const ImgPtrs_t sImgPtrs, Img_t* pOutImg, void* pMArg_CCM);

void test_ccm_hash();

#endif