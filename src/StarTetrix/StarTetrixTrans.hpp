#ifndef _STAR_TETRIX_TRANS_H
#define _STAR_TETRIX_TRANS_H

#include "../Infra/ImgDef.hpp"
#include "../Filter/SlidingWindow.hpp"

typedef struct{
    int Wr;
    int Wb;
} StarTetrixArg_t;

void star_tetrix_forward(Img_t* pInImg, void* pStarTetrixArg);

void test_star_tetrix();
void test_star_tetrix_forward();
#endif