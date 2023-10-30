#ifndef _CONV_H
#define _CONV_H

#include <iostream>
#include <stdlib.h>
#include "../Infra/ImgDef.hpp"
#include "../Infra/RandImageGen.hpp"

typedef enum{
    ZERO,
    PERIODIC,
    MIRROR
} PADDING;

typedef struct{
    size_t panelId; // ROI is usually 2d, panel is as defined in Img_t
    size_t startRow; // starting row of original image
    size_t startCol; // starting column of original image
    size_t roiWidth; // in pixel
    size_t roiHeight; // in pixel
} ROI_t;

typedef struct{
    uint8_t* pKernel; // pointer to kernel data; may convert to any type when used.
    int nRows;
    int nCols;
    PADDING padding;
    int rowStep;
    int colStep;
    bool needFlip; // if true, it is convolution; if false, it is correlation.
} KernelCfg_t;


void test_conv();

#endif