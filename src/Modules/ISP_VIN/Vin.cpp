#include "Vin.hpp"
#include <iostream>

Hash_t default_vin_arg_hash(){
    std::string path = "../data/rawData.raw";
    int frameInd = 0;
    const char* imageFormat = get_image_format_name(RAW_RGGB);
    int width = 4256;
    int height = 2848;
    int bitDepth = 16;
    int alignment = 1;

    Hash_t subHs = {
        {"path", path},
        {"frameInd", frameInd},
        {"imageFormat", imageFormat},
        {"width", width},
        {"height", height},
        {"bitDepth", bitDepth},
        {"alignment", alignment}
    };

    bool rewind = true;
    Hash_t hs = {
        {"ReadRawArg", subHs},
        {"rewind", rewind}
    };
    return hs;
}

IMG_RTN_CODE isp_vin(const ImgPtrs_t sInImgPtrs, Img_t* pOutImg, void* pMArg_Vin){
    MArg_Vin_t* pMArg = (MArg_Vin_t*)pMArg_Vin;
    // Input Img: don't care.
    read_raw_to_img_t(pMArg->sReadRawArg.path.c_str(), // convert std::string to const char*
                       pOutImg,
                       pMArg->sReadRawArg.imageFormat,
                       pMArg->sReadRawArg.width,
                       pMArg->sReadRawArg.height,
                       pMArg->sReadRawArg.bitDepth,
                       pMArg->sReadRawArg.alignment,
                       pMArg->sReadRawArg.frameInd);

    pOutImg->sign = UNSIGNED; // just to make sure that it is unsigned.

    if (!(pMArg->rewind)){
        pMArg->sReadRawArg.frameInd += 1;
    }
    
    return SUCCEED;
}


void test_hash(){
    Hash_t argHashVin = default_vin_arg_hash();
    print_hash(&argHashVin);
}