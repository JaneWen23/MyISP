#ifndef _VIN_H
#define _VIN_H

#include "../COMMON/common.hpp"
#include "../../Algos/Infra/ImageIO.hpp"

typedef struct{
    ReadRawArg_t sReadRawArg;
    bool rewind;
} MArg_Vin_t;


Hash_t default_vin_arg_hash();
const MArg_Vin_t get_vin_arg_struct_from_hash(Hash_t* pHs);
IMG_RTN_CODE isp_vin(const ImgPtrs_t sImgPtrs, Img_t* pOutImg, Hash_t* pMArg_Vin);

void test_hash();

#endif