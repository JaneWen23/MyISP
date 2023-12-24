#ifndef _COMMON_H
#define _COMMON_H

#include "../../Algos/Infra/ImgDef.hpp"


typedef struct{ // input images for a module
    Img_t* pMainImg;
    Img_t* pAddlImg1; // additional image 1
    Img_t* pAddlImg2; // additional image 2
    Img_t* pAddlImg3; // additional image 3
} ImgPtrs_t;

typedef enum {
    ISP_NONE,
    ISP_VIN,
    ISP_COMPRESSION,
    ISP_BLC,
    ISP_DMS,
    ISP_WB,
    ISP_CCM,
    ISP_RGB2YUV
} MODULE_ENUM;

const char* get_module_name(const MODULE_ENUM m);

#endif