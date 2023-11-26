#ifndef _STAR_TETRIX_TRANS_H
#define _STAR_TETRIX_TRANS_H

#include "../Infra/ImgDef.hpp"
#include "../Filter/SlidingWindow.hpp"

typedef struct{
    int Wr;
    int Wb;
} StarTetrixArg_t;

IMG_RTN_CODE star_tetrix_forward(Img_t* pInImg, Img_t* pOutImg, void* pStarTetrixArg);
IMG_RTN_CODE star_tetrix_backward(Img_t* pInImg, Img_t* pOutImg, void* pStarTetrixArg);

void demo_star_tetrix();
void test_star_tetrix();
#endif