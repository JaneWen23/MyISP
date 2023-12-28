#ifndef _COMMON_H
#define _COMMON_H

#include "../../Algos/Infra/ImgDef.hpp"
#include <vector>

typedef struct{ // input images for a module
    Img_t* pMainImg;
    Img_t* pAddlImg1; // additional image 1
    Img_t* pAddlImg2; // additional image 2
    Img_t* pAddlImg3; // additional image 3
} ImgPtrs_t;

typedef enum {
    DUMMY0,
    DUMMY1,
    DUMMY2,
    DUMMY3,
    DUMMY4,
    DUMMY5,
    DUMMY6,
    DUMMY7,
    DUMMY8,
    ISP_VIN,
    ISP_COMPRESSION,
    ISP_BLC,
    ISP_DMS,
    ISP_WB,
    ISP_CCM,
    ISP_RGB2YUV
} MODULE_NAME;


typedef struct{
    int a;
} MArg_Dummy_t;

IMG_RTN_CODE isp_dummy(const ImgPtrs_t sInImgPtrs, Img_t* pOutImg, void* pMArg_Dummy);

const char* get_module_name(const MODULE_NAME m);

#endif