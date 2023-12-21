#ifndef _VIN_H
#define _VIN_H

#include "../COMMON/common.hpp"
#include "../../Algos/Infra/ImageIO.hpp"

typedef struct{
    ReadRawArg_t sReadRawArg;
    bool rewind;
} MArg_Vin_t;


IMG_RTN_CODE isp_vin(const ImgPtrs_t sImgPtrs, Img_t* pOutImg, void* pMArg_Vin);


#endif