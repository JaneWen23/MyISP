#include "Vin.hpp"
#include <iostream>

Hash_t default_vin_arg_hash(){
    std::string path = "../data/rawData.raw";
    int frameInd = 0;
    std::string imageFormat = get_image_format_name(RAW_RGGB);
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

const MArg_Vin_t get_vin_arg_struct_from_hash(Hash_t* pHs){
    Hash_t* pSubHs = std::any_cast<Hash_t>(&(*pHs).at("ReadRawArg"));
    ReadRawArg_t sReadRawArg = {};
    sReadRawArg.path = std::any_cast<std::string>((*pSubHs).at("path"));
    sReadRawArg.frameInd = std::any_cast<int>((*pSubHs).at("frameInd"));
    sReadRawArg.imageFormat = get_image_format_from_name(std::any_cast<std::string>((*pSubHs).at("imageFormat")));
    sReadRawArg.width = std::any_cast<int>((*pSubHs).at("width"));
    sReadRawArg.height = std::any_cast<int>((*pSubHs).at("height"));
    sReadRawArg.bitDepth = std::any_cast<int>((*pSubHs).at("bitDepth"));
    sReadRawArg.alignment = std::any_cast<int>((*pSubHs).at("alignment"));

    return{
        sReadRawArg,
        std::any_cast<bool>((*pHs).at("rewind"))
    };
}

IMG_RTN_CODE isp_vin(const ImgPtrs_t sInImgPtrs, Img_t* pOutImg, Hash_t* pHs, bool updateArgs){
    MArg_Vin_t sMArg = get_vin_arg_struct_from_hash(pHs);
    // Input Img: don't care.
    read_raw_to_img_t(sMArg.sReadRawArg.path.c_str(), // convert std::string to const char*
                       pOutImg,
                       sMArg.sReadRawArg.imageFormat,
                       sMArg.sReadRawArg.width,
                       sMArg.sReadRawArg.height,
                       sMArg.sReadRawArg.bitDepth,
                       sMArg.sReadRawArg.alignment,
                       sMArg.sReadRawArg.frameInd);

    pOutImg->sign = UNSIGNED; // just to make sure that it is unsigned.

    // update arg hash by algorithm (if updateArgs == false, it means to use cfg for next frame's args)
    if (updateArgs){
        if (!(sMArg.rewind)){
            set_hash_at_path(pHs, {"ReadRawArg", "frameInd"}, sMArg.sReadRawArg.frameInd + 1);
        }
    }
    
    return SUCCEED;
}


void test_hash(){
    Hash_t argHashVin = default_vin_arg_hash();
    print_hash(&argHashVin);
}