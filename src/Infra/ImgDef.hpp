#ifndef _ImgDef_H
#define _ImgDef_H

#include <stdint.h>
#include <stddef.h>

#define MAX_NUM_P 4 // max number of panels

typedef enum{
    MONO,
    RGB,
    RAW_RGGB,
    RAW_GRGB,
    RAW_GBGR,
    RAW_BGGR
} IMAGE_FMT;

typedef struct{
    // NOTE: only integer (signed or unsigned) image data is supported.
    IMAGE_FMT imageFormat;
    size_t width; // image width in pixels; there may be different widths for different panels, to be handled by imageFormat.
    size_t height; // image height of images; there may be different heights for different panels, to be handled by imageFormat.
    size_t bitDepth; // recommend to set to the bit depth used in COMPUTER, for example 8, 16, 32; can be signed or unsigned.
    size_t alignment; // the number of bytes for a line is a multiple of number of alignment, in bytes; if no specific alignment, set this to 1.
    size_t strides[MAX_NUM_P]; // in bytes
    uint8_t* pImageData[MAX_NUM_P]; // the pointers to all components of the image
} Img_t;

typedef enum{
    SUCCEED,
    ALLOCATION_FAIL,
    INVALID_INPUT,
    PROCESS_INCOMPLETE
} IMG_RTN_CODE;



IMG_RTN_CODE construct_img(Img_t* pImg, 
                            IMAGE_FMT imageFormat,
                            size_t width,
                            size_t height,
                            size_t bitDepth,
                            size_t alignment,
                            bool allocateImage);
IMG_RTN_CODE destruct_img(Img_t**);
void view_img_properties(const Img_t* pImg);

void test_img_def(); // to be rewritten as unit test

#endif