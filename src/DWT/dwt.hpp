#ifndef _DWT_H
#define _DWT_H

#include "../Infra/ImgDef.hpp"
#include "../Filter/SlidingWindow.hpp"

typedef struct{

} DWTArg_t;


void test_dwt();

IMG_RTN_CODE dwt_forward_1d(Img_t* pInImg, Img_t* pOutImg, void* pAlgoArg);

#endif