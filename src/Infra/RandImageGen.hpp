#ifndef _RandImageGen_H
#define _RandImageGen_H

#include <stdint.h>
#include <stddef.h>
#include "ImgDef.hpp"

typedef struct{
    int lb; // uniform distribution lower bound; if distribution is actually a constant, this will be returned.
    int ub; // uniform distribution upper bound.
    //  the range is defined as [lb, ub], as a closed interval.
    int mu; // this is for gaussian distribution, can set randomly if do not generate normal-distributed random numbers.
    int sigma; // this is for gaussian distribution, can set randomly if do not generate normal-distributed random numbers.
} Distrib_t;

typedef struct{
    SIGN sign; // SIGN is defined in "ImgDef.hpp"
    const int (*FGetNum)(const Distrib_t&); // function to generate the number under a certain distribution.
    Distrib_t sDistrib;
} ValCfg_t;

const int constant_num(const Distrib_t& sDistrib);
const int rand_num_uniform(const Distrib_t& sDistrib);

IMG_RTN_CODE set_value(Img_t* pImg, const ValCfg_t& sValCfg);
void test_rand_image_gen();

#endif