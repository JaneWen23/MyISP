#ifndef _CONV_H
#define _CONV_H

#include <iostream>
#include <stdlib.h>
#include "../Infra/ImgDef.hpp"
#include "../Infra/RandImageGen.hpp"

typedef enum{
    ZEROPADDING,
    PERIODIC,
    MIRROR
} PADDING;

typedef struct{
    uint8_t* pKernel; // pointer to kernel data; may convert to any type when used.
    int nRows;
    int nCols;
    int centerX; // TODO: give a better name; also, consider 2D situation.
    PADDING padding;
    int rowStep;
    int colStep;
    bool needFlip; // if true, it is convolution; if false, it is correlation.
} KernelCfg_t;


void test_conv();

#endif