#ifndef _COMMON_H
#define _COMMON_H

#include "../../Algos/Infra/ImgDef.hpp"


typedef struct{ // input images for a module
    Img_t* pMainImg;
    Img_t* pAddlImg1; // additional image 1
    Img_t* pAddlImg2; // additional image 2
    Img_t* pAddlImg3; // additional image 3
} ImgPtrs_t;


#endif