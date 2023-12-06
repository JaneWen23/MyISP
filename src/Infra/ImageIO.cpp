#include <iostream>
#include "ImageIO.hpp"
#include "../Infra/RandImageGen.hpp"


void read_raw_to_img_t(const char* path,
                       Img_t* pImg,
                       const IMAGE_FMT imageFormat,
                       const int width,
                       const int height,
                       const int bitDepth,
                       const int alignment){
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
    fread(pImg->pImageData[0], sizeof(uint8_t), pImg->strides[0] * height, pFile);
    fclose(pFile);
}


