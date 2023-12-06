#include <iostream>
#include "ImageIO.hpp"
#include "../Infra/RandImageGen.hpp"


IMG_RTN_CODE read_raw_to_img_t(const char* path,
                       Img_t* pImg,
                       const IMAGE_FMT imageFormat,
                       const int width,
                       const int height,
                       const int bitDepth,
                       const int alignment,
                       int frameInd){
    // NOTE: frameInd is optional and default is 0, 
    // meaning that read from the beginning (0th frame) of the file
    switch (imageFormat){
        case RAW_RGGB:
        case RAW_GRBG:
        case RAW_GBRG:
        case RAW_BGGR:{
            break; // nothing is wrong, just go ahead
        }
        default:{
            std::cout<<"error: image format is not raw. exited.\n";
            exit(1);
        }
    }
    FILE* pFile = NULL;
    pFile = fopen(path, "r");
    if (pFile == NULL){
        std::cout<<"error: fail to open the file. exited\n";
        exit(1);
    }
    construct_img(pImg, 
                imageFormat,
                width,
                height,
                UNSIGNED,
                bitDepth,
                alignment,
                true);
    long int frameSize = pImg->strides[0] * height; // how many bytes in one frame
    fseek(pFile, frameSize, SEEK_SET);
    fread(pImg->pImageData[0], sizeof(uint8_t), frameSize, pFile);
    fclose(pFile);
    return SUCCEED;
}


IMG_RTN_CODE read_frame(const Img_t* pInImg, Img_t* pOutImg, const void* pReadRawArg){
    // pInImg should be NULL since no input yet
    assert(pInImg == NULL);
    ReadRawArg_t* pArg = (ReadRawArg_t*)pReadRawArg;
    read_raw_to_img_t(pArg->path,
                       pOutImg,
                       pArg->imageFormat,
                       pArg->width,
                       pArg->height,
                       pArg->bitDepth,
                       pArg->alignment,
                       pArg->frameInd);

    return SUCCEED;
}