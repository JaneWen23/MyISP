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
    size_t start_row; // starting row of original image
    size_t start_col; // starting column of original image
    size_t roi_width; // in pixel
    size_t roi_height; // in pixel
} ROI_t;

void test_conv();

#endif