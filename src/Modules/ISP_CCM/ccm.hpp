#ifndef _CCM_H
#define _CCM_H

#include "../COMMON/common.hpp"
#include "../../Algos/Color/Color.hpp"

typedef struct{
    CCMArg_t sCCMArg;
} MArg_CCM_t;


IMG_RTN_CODE isp_ccm(const ImgPtrs_t sImgPtrs, Img_t* pOutImg, void* pMArg_CCM);

#endif