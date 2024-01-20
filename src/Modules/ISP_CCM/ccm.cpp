#include "ccm.hpp"

Hash_t default_ccm_arg_hash(){
    int colorMatRow1[3] = {278, -10, -8};
    int colorMatRow2[3] = {-12, 269, -8};
    int colorMatRow3[3] = {-10, -3, 272};
    Hash_t Row1 = {
        {"RR", colorMatRow1[0]},
        {"RG", colorMatRow1[1]},
        {"RB", colorMatRow1[2]}
    };
    Hash_t Row2 = {
        {"GR", colorMatRow2[0]},
        {"GG", colorMatRow2[1]},
        {"GB", colorMatRow2[2]}
    };
    Hash_t Row3 = {
        {"BR", colorMatRow3[0]},
        {"BG", colorMatRow3[1]},
        {"BB", colorMatRow3[2]}
    };

    Hash_t CCMArg = {
        {"ColorMatRow1", Row1},
        {"ColorMatRow2", Row2},
        {"ColorMatRow3", Row3}
    };
    Hash_t hs = {{"CCMArg", CCMArg}};
    return hs;
}

const MArg_CCM_t get_ccm_arg_struct_from_hash(Hash_t* pHs){
    Hash_t* pSubHs1 = std::any_cast<Hash_t>(&(*pHs).at("CCMArg"));

    Hash_t* pSubHs11 = std::any_cast<Hash_t>(&(*pSubHs1).at("ColorMatRow1"));
    ColorMatRow1_t sRow1 = {};
    sRow1.RR = std::any_cast<int>((*pSubHs11).at("RR"));
    sRow1.RG = std::any_cast<int>((*pSubHs11).at("RG"));
    sRow1.RB = std::any_cast<int>((*pSubHs11).at("RB"));

    Hash_t* pSubHs12 = std::any_cast<Hash_t>(&(*pSubHs1).at("ColorMatRow2"));
    ColorMatRow2_t sRow2 = {};
    sRow2.GR = std::any_cast<int>((*pSubHs12).at("GR"));
    sRow2.GG = std::any_cast<int>((*pSubHs12).at("GG"));
    sRow2.GB = std::any_cast<int>((*pSubHs12).at("GB"));

    Hash_t* pSubHs13 = std::any_cast<Hash_t>(&(*pSubHs1).at("ColorMatRow3"));
    ColorMatRow3_t sRow3 = {};
    sRow3.BR = std::any_cast<int>((*pSubHs13).at("BR"));
    sRow3.BG = std::any_cast<int>((*pSubHs13).at("BG"));
    sRow3.BB = std::any_cast<int>((*pSubHs13).at("BB"));

    CCMArg_t sCCMArg = {
        sRow1,
        sRow2,
        sRow3
    };

    return {
        sCCMArg
    };
}

IMG_RTN_CODE isp_ccm(const ImgPtrs_t sInImgPtrs, Img_t* pOutImg, Hash_t* pHs, bool updateArgs){
    MArg_CCM_t sMArg = get_ccm_arg_struct_from_hash(pHs);
    CCMRunTimeArg_t sRunTimeArg = {
        {sMArg.sCCMArg.sColorMatRow1.RR, sMArg.sCCMArg.sColorMatRow1.RG, sMArg.sCCMArg.sColorMatRow1.RB},
        {sMArg.sCCMArg.sColorMatRow2.GR, sMArg.sCCMArg.sColorMatRow2.GG, sMArg.sCCMArg.sColorMatRow2.GB},
        {sMArg.sCCMArg.sColorMatRow3.BR, sMArg.sCCMArg.sColorMatRow3.BG, sMArg.sCCMArg.sColorMatRow3.BB}
    };

    if (sInImgPtrs[0]->imageFormat != RGB){
        std::cout<<"error: ISP_CCM: input image not being RGB. exited.\n";
        exit(1);
    }
    safe_unsigned_to_signed_img(sInImgPtrs[0]); // ccm() is defined to be signed.

    ccm(sInImgPtrs[0], pOutImg, &sRunTimeArg);
    if (pOutImg->imageFormat != RGB){
        std::cout<<"error: ISP_CCM: output image not being RGB. exited.\n";
        exit(1);
    }
    
    safe_signed_to_unsigned_img(pOutImg); // modules only allow unsigned data.
    
    if (updateArgs){
        // TODO
    }
    
    return SUCCEED;
}

void test_ccm_hash(){
    Hash_t hs = default_ccm_arg_hash();
    print_hash(&hs);
}