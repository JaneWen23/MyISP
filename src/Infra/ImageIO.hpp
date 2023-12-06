#ifndef _IMAGE_IO_H
#define _IMAGE_IO_H
#include "ImgDef.hpp"


typedef struct{
    char* path;
    IMAGE_FMT imageFormat;
    int width;
    int height;
    int bitDepth;
    int alignment;
} ReadRawArg_t;

IMG_RTN_CODE read_raw_to_img_t(const char* path,
                       Img_t* pImg,
                       const IMAGE_FMT imageFormat,
                       const int width,
                       const int height,
                       const int bitDepth,
                       const int alignment);

IMG_RTN_CODE read_frame(const Img_t* pInImg, Img_t* pOutImg, const void* pReadRawArg);

#endif