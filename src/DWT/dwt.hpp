#ifndef _dwt_H
#define _dwt_H

#include "../Infra/ImgDef.hpp"

void test_dwt();

IMG_RTN_CODE dwt_forward_1d(Img_t* pInImg, Img_t* pOutImg, void* pAlgoArgs);

#endif