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
    int horiStep; // horizontal step
    int vertStep; // vertical step
    bool needFlip; // if true, it is convolution; if false, it is correlation.
    void* formula;
} KernelCfg_t;


void test_conv();

#endif