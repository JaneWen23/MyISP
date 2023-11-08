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
    int kerHeight;
    int kerWidth;
    int horiCenter; // horizontal center
    int vertCenter; // vertical center
    PADDING padding;
    int horiStep; // horizontal step (same effect as filtering followed by down-sampling)
    int vertStep; // vertical step (same effect as filtering followed by down-sampling)
    int horiUpsample; // (down-sampled (or not)) filtering followed by up-sampling
    int vertUpsample; // (down-sampled (or nor)) filtering followed by up-sampling
    bool needFlip; // if true, it is convolution; if false, it is correlation.
    void* formula;
    bool divideAndConquer; // if true, 2d filtering can be defined by manipulations of several 1d results; if false, 2d filtering may have a different formula than 1d.
} KernelCfg_t;


void test_conv();

#endif