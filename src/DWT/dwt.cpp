#include "stdlib.h"
#include "iostream"
#include "dwt.hpp"
//#include "../Infra/ImgDef.hpp"

using namespace std;

IMG_RTN_CODE dwt_forward_1d(Img_t* pImg, Img_t* pOutImg, void* pAlgoArgs){
    return SUCCEED;
}


void test_dwt(){
    // should use allocate_image().
    Img_t* pMyImg = NULL; // initialze
    pMyImg =(Img_t*)malloc(sizeof(Img_t*));
    if (pMyImg == NULL){
        std::cout<<"fail to allocat Img_t object\n";
        return;
    }
    pMyImg->bitDepth = 10;
    pMyImg->height = 30;
    pMyImg->width = 50;
    pMyImg->nComp = 3;
    pMyImg->strides[0] = 100; // 50 * 2(bytes)
    pMyImg->strides[1] = 100;
    pMyImg->strides[2] = 100;
    pMyImg->strides[3] = 0; // because we only have 3 channels
    
    int* pImgData = NULL;
    pImgData = (int*)malloc(pMyImg->width * pMyImg->height * ((pMyImg->bitDepth+7)/8));
    if (pMyImg == NULL){
    std::cout<<"fail to allocat image data\n";
    return;
    }

    std::cout<<pMyImg<<endl;
}