#include "Color.hpp"
#include "../Math/Math.hpp"


template<typename T>
void ccm_value_manipulation(uint8_t* pColorMat, uint8_t** pColorMatRows, const void* pCCMArg){
    CCMArg_t* pArg = (CCMArg_t*)pCCMArg;
    for (int j = 0; j < 3; ++j){
        *((T*)pColorMat + j) = (T)(*(pArg->colorMatRow1 + j));
    }
    for (int j = 0; j < 3; ++j){
        *((T*)pColorMat + 3 + j) = (T)(*(pArg->colorMatRow2 + j));
    }
    for (int j = 0; j < 3; ++j){
        *((T*)pColorMat + 6 + j) = (T)(*(pArg->colorMatRow3 + j));
    }
    ((T**)pColorMatRows)[0] = (T*)pColorMat + 0;
    ((T**)pColorMatRows)[1] = (T*)pColorMat + 3;
    ((T**)pColorMatRows)[2]= (T*)pColorMat + 6;
}

typedef void (*FMAN)(uint8_t*, uint8_t**, const void*);

template<typename T>
void config_ccm_kernel(KernelCfg_t* pKerCfg, const uint8_t** pColorMatRows){
    pKerCfg->pKernel = ((uint8_t*)pColorMatRows);
    pKerCfg->formula =  (void*)color_correction<T>;

    pKerCfg->horiStep = 1; // usually set this to 1 for ccm
    pKerCfg->vertStep = 1; // usually set this to 1 for ccm
    pKerCfg->horiUpsample = 1; // usually set this to 1 for ccm
    pKerCfg->vertUpsample = 1; // usually set this to 1 for ccm
    
    pKerCfg->kerHeight = 1; // don't care
    pKerCfg->kerWidth = 1; // don't care
    pKerCfg->horiCenter = 0; // don't care
    pKerCfg->vertCenter = 0; // don't care
    pKerCfg->padding = ZEROPADDING; // don't care, nonsense
    pKerCfg->needFlip = false; // don't care, nonsense
    pKerCfg->divideAndConquer = false; // don't care, nonsense
}

typedef void (*FCFG)(KernelCfg_t*, const uint8_t**);

void config_ccm_kernel_exact(KernelCfg_t* pKerCfg, uint8_t* pColorMat, uint8_t** pColorMatRows, const void* pCCMArg, const SIGN sign, const int bitDepth){
    FMAN g = NULL;
    FCFG f = NULL;
    if (sign == UNSIGNED){
        std::cout<<"error: CCM does not support unsigned data types. exited.\n";
        exit(1);
    }
    else{
        if (bitDepth <= 8){
            g = ccm_value_manipulation<int8_t>;
            f = config_ccm_kernel<int8_t>;
        }
        else if (bitDepth <= 16){
            g = ccm_value_manipulation<int16_t>;
            f = config_ccm_kernel<int16_t>;
        }
        else if (bitDepth <= 32){
            g = ccm_value_manipulation<int>;
            f = config_ccm_kernel<int>;
        }
    }
    g(pColorMat, pColorMatRows, pCCMArg);
    f(pKerCfg, (const uint8_t**)pColorMatRows);
}



IMG_RTN_CODE ccm(const Img_t* pInImg, Img_t* pOutImg, const void* pCCMArg){
    // config the 1x1 kernel:
    KernelCfg_t* pKerCfg = (KernelCfg_t*)malloc(sizeof(KernelCfg_t));
    uint8_t* pColorMat = (uint8_t*)malloc(9 * sizeof(int)); // 3 rows, 3 columns; not all are used since actual data type may be int16
    uint8_t* pColorMatRows[3*sizeof(int)] = {NULL};
    config_ccm_kernel_exact(pKerCfg, pColorMat, pColorMatRows, pCCMArg, pOutImg->sign, pOutImg->bitDepth);

    // config the in and out img ROI:
    ROI_t sInImgROI = {1012, 0, 0, pInImg->width, pInImg->height};
    ROI_t sOutImgROI = {1012, 0, 0, pOutImg->width, pOutImg->height};

    // apply the 1x1 sliding window:
    sliding_window_1x1(pInImg, sInImgROI, pOutImg, sOutImgROI, *pKerCfg);
    free(pKerCfg);
    free(pColorMat);
    return SUCCEED;
}


void test_ccm(){
    Img_t* pImg1 =(Img_t*)malloc(sizeof(Img_t));
    IMAGE_FMT imageFormat = RGB;
    int width = 4;
    int height = 4;
    SIGN sign = SIGNED;
    int bitDepth = 16;
    int alignment = 32;
    bool allocateImage = true;

    construct_img(pImg1, 
                  imageFormat,
                  width,
                  height,
                  sign,
                  bitDepth,
                  alignment,
                  allocateImage);
    
    ValCfg_t sValCfg = {pImg1->sign, rand_num_uniform, {0, 49, 0, 0}};

    set_value(pImg1, sValCfg);
    view_img_properties(pImg1);

    std::cout<<"original:\n";
    ROI_t viewROI_1 = {1012,0,0,width,height}; // 1012 means panel 0, 1, 2 are all chosen
    view_image_data(pImg1, viewROI_1);

    Img_t* pImg2 =(Img_t*)malloc(sizeof(Img_t));
    construct_img(pImg2, 
                  imageFormat,
                  width,
                  height,
                  sign,
                  bitDepth,
                  alignment,
                  allocateImage);

    const CCMArg_t CCMArg = {
        {278, -10, -8},
        {-12, 269, -8},
        {-10, -3, 272},
    };

    ccm(pImg1, pImg2, (void*)&CCMArg);

    std::cout<<"filtered:\n";
    ROI_t viewROI_2 = {1012,0,0,width,height};
    view_image_data(pImg2, viewROI_2);

    destruct_img(&pImg1);
    destruct_img(&pImg2);
}

void rgb_to_yuv420(const Img_t* pInImg, Img_t* pOutImg, const void* pCSCArg){
    CSCArg_t* pArg = (CSCArg_t*)pCSCArg;

    const KernelCfg_t sKernelCfg = {NULL, // 
                                    1, 1, 0, 0, // don't care for 1x1 window
                                    ZEROPADDING, // nonsense
                                    1, 1, 1, 1, // usually keep them all 1's for 1x1 window
                                    false,  // nonsense
                                    pArg->color_conversion_formula, // in use
                                    false // nonsense
                                    };

    ROI_t sInImgROI = {1012, 0, 0, pInImg->width, pInImg->height};
    ROI_t sOutImgROI = {1012, 0, 0, pOutImg->width, pOutImg->height};

    sliding_window_1x1(pInImg, sInImgROI, pOutImg, sOutImgROI, sKernelCfg);
}

void rgb_to_yuv420_prototype(){
    Img_t* pImg1 =(Img_t*)malloc(sizeof(Img_t));
    IMAGE_FMT imageFormat = RGB;
    int width = 4;
    int height = 4;
    SIGN sign = SIGNED;
    int bitDepth = 32;
    int alignment = 32;
    bool allocateImage = true;

    construct_img(pImg1, 
                  imageFormat,
                  width,
                  height,
                  sign,
                  bitDepth,
                  alignment,
                  allocateImage);
    
    ValCfg_t sValCfg = {pImg1->sign, rand_num_uniform, {0, 49, 0, 0}};

    set_value(pImg1, sValCfg);
    view_img_properties(pImg1);

    std::cout<<"original:\n";
    ROI_t viewROI_1 = {1012,0,0,width,height}; // 1012 means panel 0, 1, 2 are all chosen
    view_image_data(pImg1, viewROI_1);

    Img_t* pImg2 =(Img_t*)malloc(sizeof(Img_t));
    construct_img(pImg2, 
                  YUV420,
                  width,
                  height,
                  sign,
                  bitDepth,
                  alignment,
                  allocateImage);

    Formulas_T<int> Fml;
    Fml.f = rgb_to_yuv_bt709;

    // const KernelCfg_t sKernelCfg = {NULL, // 
    //                                 1, 1, 0, 0, // don't care for 1x1 window
    //                                 ZEROPADDING, // nonsense
    //                                 1, 1, 1, 1, // usually keep them all 1's for 1x1 window
    //                                 false,  // nonsense
    //                                 (void*)Fml.f, // in use
    //                                 false // nonsense
    //                                 };

    // ROI_t sInImgROI = {1012, 0, 0, width, height}; // 1012 means panel 0, 1, 2 are all chosen
    // ROI_t sOutImgROI = {1012, 0, 0, width, height};

    // sliding_window_1x1(pImg1, sInImgROI, pImg2, sOutImgROI, sKernelCfg);

    std::cout<<"filtered:\n";
    ROI_t viewROI_2 = {1012,0,0,width,height};
    view_image_data(pImg2, viewROI_2);

    destruct_img(&pImg1);
    destruct_img(&pImg2);
}

// TODO: WBGain, RGB GAMMA, Y GAMMA