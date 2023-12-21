#ifndef _IMAGE_IO_H
#define _IMAGE_IO_H
#include "ImgDef.hpp"


typedef struct{
    const char* path;
    int frameInd; // read i-th frame, i >= 0
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
                       const int alignment,
                       int frameInd = 0);


#endif