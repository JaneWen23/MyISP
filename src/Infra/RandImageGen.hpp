#ifndef _RandImageGen_H
#define _RandImageGen_H

#include <stdint.h>
#include <stddef.h>
#include "ImgDef.hpp"

typedef struct{
    bool isSigned;
    int constant;
    // uniform distri
    // gaussian distri
} ValCfg_t;


void test_rand_image_gen();

#endif