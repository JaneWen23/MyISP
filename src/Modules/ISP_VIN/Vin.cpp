#include "Vin.hpp"
#include <iostream>

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
    hash_t hs;
    std::any path = "../data/rawData.raw";
    std::any frameInd = 0;
    std::any imageFormat = get_image_format_name(MONO);
    std::any width = 100;
    std::any height = 100;
    std::any bitDepth = 8;
    std::any alignment = 32;
    hs.insert({"path", path});
    hs.insert({"frameInd", frameInd});
    hs.insert({"imageFormat", imageFormat});
    hs.insert({"width", width});
    hs.insert({"height", height});
    hs.insert({"bitDepth", bitDepth});
    hs.insert({"alignment", alignment});


    // std::cout << "path: " << std::any_cast<const char*>(hs.at("path")) << '\n'; // why cannot any_cast<std::string>??????
    // std::cout << "frameInd: " << std::any_cast<int>(hs.at("frameInd")) << '\n';
    // std::cout << "imageFormat: " << std::any_cast<const char*>(hs.at("imageFormat"))<<"\n";
    // std::cout << "width: " << std::any_cast<int>(hs.at("width")) << '\n';
    // std::cout << "height: " << std::any_cast<int>(hs.at("height")) << '\n';
    // std::cout << "bitDepth: " << std::any_cast<int>(hs.at("bitDepth")) << '\n';
    // std::cout << "alignment: " << std::any_cast<int>(hs.at("alignment")) << '\n';

    ReadRawArg_t sReadRawArg = {
        std::any_cast<const char*>(hs.at("path")), // why cannot any_cast<std::string>??????
        std::any_cast<int>(hs.at("frameInd")),
        get_image_format_from_name(std::any_cast<const char*>(hs.at("imageFormat"))),
        std::any_cast<int>(hs.at("width")),
        std::any_cast<int>(hs.at("height")),
        std::any_cast<int>(hs.at("bitDepth")),
        std::any_cast<int>(hs.at("alignment"))
    };

    hash_t shVin;
    bool rewind = false;
    shVin = {{"sReadRawArg", sReadRawArg}, {"rewind", rewind}};

    ReadRawArg_t s1 =  std::any_cast<ReadRawArg_t>(shVin.at("sReadRawArg"));
    std::cout<< s1.path<<"\n";
    std::cout<< std::any_cast<bool>(shVin.at("rewind")) <<"\n";

    hash_t shAll;
    shAll.insert({"sVinArg", shVin});
}