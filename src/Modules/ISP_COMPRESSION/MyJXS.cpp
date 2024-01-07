#include "MyJXS.hpp"

// this is only a prototype: NO compression,
// forward is forward star-tetrix + dwt, 
// and backward is idwt + backward star-tetrix

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


IMG_RTN_CODE isp_compression(const ImgPtrs_t sInImgPtrs, Img_t* pOutImg, void* pMyJXSArg){
    safe_unsigned_to_signed_img(sInImgPtrs[0]);

    Img_t* pTmpImg = (Img_t*)malloc(sizeof(Img_t));
    my_jxs_forward(sInImgPtrs[0], pTmpImg, pMyJXSArg);
    my_jxs_backward(pTmpImg, pOutImg, pMyJXSArg);
    destruct_img(&pTmpImg);
    // currently, the module execution will not update any arguments, i.e., pMyJXSArg will not be changed by this func.
    // there may be some adaptive parameters if perform a "real" compression algo (;TODO)
    // also, arguments may be changed by the provided arguments from toml latter.
    
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



    StarTetrixArg_t sStarTetrixArg = {
        1,
        2
    };

    DWTArg_t sDWTArg = {
        0, // int inImgPanelId; // apply dwt to the whole 2D image
        0, // int outImgPanelId;
        TWO_DIMENSIONAL, // ORIENT orient;
        1, // int level;
        LE_GALL_53, // WAVELET_NAME wavelet;
        MIRROR // PADDING padding;
    };

    MArg_Compression_t* pMyJXSArg = (MArg_Compression_t*)malloc(sizeof(MArg_Compression_t));
    pMyJXSArg->sStarTetrixArg = sStarTetrixArg;
    pMyJXSArg->sDWTArg = sDWTArg;

    ImgPtrs_t sImgPtrs(1);
    sImgPtrs[0] = pInImg;

    std::cout<<"original:\n";
    ROI_t viewROI = {0, 0, 0, pInImg->width, pInImg->height};
    view_image_data(pInImg, viewROI );

    isp_compression(sImgPtrs, pOutImg, pMyJXSArg);

    std::cout<<"pipe out:\n";
    ROI_t viewROI3 = {0, 0, 0, pOutImg->width, pOutImg->height};
    view_image_data(pOutImg, viewROI3);
    view_img_properties(pOutImg);

    destruct_img(&pInImg);
    destruct_img(&pOutImg);
}
