#ifndef _ImgDef_H
#define _ImgDef_H

#include <stdint.h>
#include <stddef.h>

#define MAX_NUM_P 4 // max number of panels

typedef enum{
    MONO,
    RGB,
    RAW_RGGB,
    RAW_GRBG,
    RAW_GBRG,
    RAW_BGGR,
    YUV420,
    Y_C_C_D_RGGB, // the star-tetrix transformed format, Y_C_C_D means Ybar, Cb, Cr, Delta in four panels, RGGB means it was RGGB.
    Y_C_C_D_GRBG, // the star-tetrix transformed format, Y_C_C_D means Ybar, Cb, Cr, Delta in four panels, GRBG means it was GRBG.
    Y_C_C_D_GBRG, // the star-tetrix transformed format, Y_C_C_D means Ybar, Cb, Cr, Delta in four panels, GBRG means it was GBRG.
    Y_C_C_D_BGGR, // the star-tetrix transformed format, Y_C_C_D means Ybar, Cb, Cr, Delta in four panels, BGGR means it was BGGR.
    UNKOWN
} IMAGE_FMT;

typedef enum{
    UNSIGNED,
    SIGNED
} SIGN;

typedef struct{
    // NOTE: only integer (signed or unsigned) image data is supported.
    IMAGE_FMT imageFormat;
    int width; // image width in pixels; there may be different widths for different panels, to be handled by imageFormat.
    int height; // image height of images; there may be different heights for different panels, to be handled by imageFormat.
    SIGN sign;
    int bitDepth; // recommend to set to the bit depth used in COMPUTER, for example 8, 16, 32; can be signed or unsigned.
    int alignment; // the number of bytes for a line is a multiple of number of alignment, in bytes; if no specific alignment, set this to 1.
    int strides[MAX_NUM_P]; // in bytes
    uint8_t* pImageData[MAX_NUM_P]; // the pointers to all components of the image
} Img_t;

typedef struct{
    int panelId; // ROI is usually 2d, panel is as defined in Img_t
    int startRow; // starting row of original image
    int startCol; // starting column of original image
    int roiWidth; // in pixel
    int roiHeight; // in pixel
} ROI_t;

typedef enum{
    SUCCEED,
    ALLOCATION_FAIL,
    INVALID_INPUT,
    PROCESS_INCOMPLETE
} IMG_RTN_CODE;



IMG_RTN_CODE construct_img(Img_t* pImg, 
                            const IMAGE_FMT imageFormat,
                            const int width,
                            const int height,
                            const SIGN sign,
                            const int bitDepth,
                            const int alignment,
                            const bool allocateImage);
IMG_RTN_CODE destruct_img(Img_t**);
void view_img_properties(const Img_t* pImg);
void view_image_data(const Img_t* pImg, const ROI_t& viewROI);
IMG_RTN_CODE duplicate_img(const Img_t* pSrcImg, Img_t* pDstImg);
// TODO: crop_image()
IMG_RTN_CODE change_img_bitDepth(const Img_t* pSrcImg, const int dstBitDepth, bool createNew, Img_t* pDstImg = NULL);
bool is_image_equal(const Img_t* pSrcImg, const Img_t* pDstImg);

void test_img_def(); // to be rewritten as unit test

#endif