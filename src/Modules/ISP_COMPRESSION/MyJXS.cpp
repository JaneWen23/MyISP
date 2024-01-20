#include "MyJXS.hpp"

// this is only a prototype: NO compression,
// forward is forward star-tetrix + dwt, 
// and backward is idwt + backward star-tetrix

Hash_t default_compression_arg_hash(){
    int Wr = 0;
    int Wb = 0;
    Hash_t StarTetrixArg = {
        {"Wr", Wr},
        {"Wb", Wb}
    };

    int inImgPanelId = 0; // apply dwt to the whole 2D image
    int outImgPanelId = 0;
    std::string orient = get_dwt_orient_name(TWO_DIMENSIONAL);
    int level = 1;
    std::string wavelet = get_wavelet_name(LE_GALL_53);
    std::string padding = get_padding_name(MIRROR);
    Hash_t DWTArg = {
        {"inImgPanelId", inImgPanelId},
        {"outImgPanelId", outImgPanelId},
        {"orient", orient},
        {"level", level},
        {"wavelet", wavelet},
        {"padding", padding}
    };

    Hash_t hs = {
        {"StarTetrixArg", StarTetrixArg},
        {"DWTArg", DWTArg}
    };
    return hs;
}

const MArg_Compression_t get_compression_arg_struct_from_hash(Hash_t* pHs){
    Hash_t* pSubHs1 = std::any_cast<Hash_t>(&(*pHs).at("StarTetrixArg"));
    StarTetrixArg_t sStarTetrixArg = {};
    sStarTetrixArg.Wr = std::any_cast<int>((*pSubHs1).at("Wr"));
    sStarTetrixArg.Wb = std::any_cast<int>((*pSubHs1).at("Wb"));

    Hash_t* pSubHs2 = std::any_cast<Hash_t>(&(*pHs).at("DWTArg"));
    DWTArg_t sDWTArg = {};
    sDWTArg.inImgPanelId = std::any_cast<int>((*pSubHs2).at("inImgPanelId"));
    sDWTArg.outImgPanelId = std::any_cast<int>((*pSubHs2).at("outImgPanelId"));
    sDWTArg.orient = get_dwt_orient_from_name(std::any_cast<std::string>((*pSubHs2).at("orient")));
    sDWTArg.level = std::any_cast<int>((*pSubHs2).at("level"));
    sDWTArg.wavelet = get_wavelet_from_name(std::any_cast<std::string>((*pSubHs2).at("wavelet")));
    sDWTArg.padding = get_padding_from_name(std::any_cast<std::string>((*pSubHs2).at("padding")));

    return {
        sStarTetrixArg,
        sDWTArg
    };
}

IMG_RTN_CODE my_jxs_forward(const Img_t* pInImg, Img_t* pOutImg, const void* pMyJXSArg){
    switch (pInImg->imageFormat){
        case RAW_RGGB:
        case RAW_GRBG:
        case RAW_GBRG:
        case RAW_BGGR:{
            break;
        }
        default:{
            std::cout<<"currently, only raw compression is supported. exited.\n";
            exit(1);
        }
    }

    MArg_Compression_t* pArg = (MArg_Compression_t*)pMyJXSArg;
    void* pStarTetrixArg = (void*)(&(pArg->sStarTetrixArg));
    star_tetrix_forward(pInImg, pOutImg, (void*)pStarTetrixArg);
    // now we have 4-paneled out img.

    DWTArg_t sDWTArg = pArg->sDWTArg;

    for(int i = 0; i < 4; ++i){
        sDWTArg.inImgPanelId = i;
        sDWTArg.outImgPanelId = i;
        dwt_forward(pOutImg, pOutImg, (void*)(&sDWTArg));
    }

    return SUCCEED;
}

IMG_RTN_CODE my_jxs_backward(const Img_t* pInImg, Img_t* pOutImg, const void* pMyJXSArg){
    switch (pInImg->imageFormat){
        case Y_C_C_D_RGGB:
        case Y_C_C_D_GRBG:
        case Y_C_C_D_GBRG:
        case Y_C_C_D_BGGR:{
            break;
        }
        default:{
            std::cout<<"currently, only raw compression is supported. exited.\n";
            exit(1);
        }
    }

    // 4-paneled in img
    MArg_Compression_t* pArg = (MArg_Compression_t*)pMyJXSArg;
    DWTArg_t sDWTArg = pArg->sDWTArg;
    Img_t* pTmpImg = (Img_t*)malloc(sizeof(Img_t));
    duplicate_img(pInImg, pTmpImg);
    for(int i = 0; i < 4; ++i){
        sDWTArg.inImgPanelId = i;
        sDWTArg.outImgPanelId = i;
        dwt_backward(pTmpImg, pTmpImg, (void*)(&sDWTArg));
    }

    void* pStarTetrixArg = (void*)(&(pArg->sStarTetrixArg));
    star_tetrix_backward(pTmpImg, pOutImg, (void*)pStarTetrixArg);
    destruct_img(&pTmpImg);
    return SUCCEED;
}


IMG_RTN_CODE isp_compression(const ImgPtrs_t sInImgPtrs, Img_t* pOutImg, Hash_t* pHs, bool updateArgs){
    MArg_Compression_t sMArg = get_compression_arg_struct_from_hash(pHs);
    safe_unsigned_to_signed_img(sInImgPtrs[0]);

    Img_t* pTmpImg = (Img_t*)malloc(sizeof(Img_t));
    my_jxs_forward(sInImgPtrs[0], pTmpImg, &sMArg);
    my_jxs_backward(pTmpImg, pOutImg, &sMArg);
    destruct_img(&pTmpImg);
    // currently, the module execution will not update any arguments, i.e., pMyJXSArg will not be changed by this func.
    // there may be some adaptive parameters if perform a "real" compression algo (;TODO)
    // also, arguments may be changed by the provided arguments from toml latter.

    if (updateArgs){
        // TODO
    }
    
    safe_signed_to_unsigned_img(pOutImg);
    return SUCCEED;
}


void test_my_jxs(){
    Img_t* pInImg =(Img_t*)malloc(sizeof(Img_t));
    IMAGE_FMT imageFormat = RAW_RGGB;
    int width = 12;
    int height = 12;
    SIGN sign = SIGNED;
    int bitDepth = 16;
    int alignment = 32;
    bool allocateImage = true; 

    construct_img(pInImg, 
                  imageFormat,
                  width,
                  height,
                  sign,
                  bitDepth,
                  alignment,
                  allocateImage); 

    ValCfg_t sValCfg = {pInImg->sign, rand_num_uniform, {0, 49, 0, 0}};

    set_value(pInImg, sValCfg);

    Img_t* pOutImg = (Img_t*)malloc(sizeof(Img_t));

    Hash_t hs = default_compression_arg_hash();

    ImgPtrs_t sImgPtrs(1);
    sImgPtrs[0] = pInImg;

    std::cout<<"original:\n";
    ROI_t viewROI = {0, 0, 0, pInImg->width, pInImg->height};
    view_image_data(pInImg, viewROI );

    isp_compression(sImgPtrs, pOutImg, &hs, false);

    std::cout<<"pipe out:\n";
    ROI_t viewROI3 = {0, 0, 0, pOutImg->width, pOutImg->height};
    view_image_data(pOutImg, viewROI3);
    view_img_properties(pOutImg);

    destruct_img(&pInImg);
    destruct_img(&pOutImg);
}
