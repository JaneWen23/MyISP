#ifndef _IMAGE_IO_H
#define _IMAGE_IO_H
#include "ImgDef.hpp"


void read_raw_to_img_t(const char* path,
                       Img_t* pImg,
                       const IMAGE_FMT imageFormat,
                       const int width,
                       const int height,
                       const int bitDepth,
                       const int alignment);


#endif