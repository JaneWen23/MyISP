#ifndef _ImgDef_H
#define _ImgDef_H

#include <stdint.h>
#include <stddef.h>

#define MAX_NUM_CH 4

typedef struct{
    size_t width; // image width in pixels
    size_t height; // image height of images
    uint8_t nComp; // numer of components
    uint8_t bitDepth;
    size_t stride[MAX_NUM_CH]; // in bytes
    uint8_t* pImgBytes[MAX_NUM_CH]; // the pointers to all components of the image
}Img_t;

typedef enum{
    SUCCEED,
    ALLOCATION_FAIL,
    PROCESS_INCOMPLETE
} IMG_RTN_CODE;

IMG_RTN_CODE allocate_img(Img_t);
IMG_RTN_CODE destruct_img(Img_t);

#endif