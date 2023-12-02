#include "Color.hpp"
#include "../Math/Math.hpp"


void ccm_prototype(){
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
                  imageFormat,
                  width,
                  height,
                  sign,
                  bitDepth,
                  alignment,
                  allocateImage);

    Formulas_1x1_T<int> Fml;
    Fml.f1x1 = color_correction;
    const int colorMatRow1[3] = {278, -10, -8};
    const int colorMatRow2[3] = {-12, 269, -8};
    const int colorMatRow3[3] = {-10, -3, 272};
    //NOTE: should make sure that sum of column is unit one (256 or so)
    const int* colorMatRows[3] = {colorMatRow1, colorMatRow2, colorMatRow3};
    const KernelCfg_t sKernelCfg = {(uint8_t*)colorMatRows, // in use
                                    1, 1, 0, 0, // don't care for 1x1 window
                                    ZEROPADDING, // nonsense
                                    1, 1, 1, 1, // usually keep them all 1's for 1x1 window
                                    false,  // nonsense
                                    (void*)Fml.f1x1, // in use
                                    false // nonsense
                                    };

    ROI_t sInImgROI = {1012, 0, 0, width, height}; // 1012 means panel 0, 1, 2 are all chosen
    ROI_t sOutImgROI = {1012, 0, 0, width, height};


    sliding_window_1x1(pImg1, sInImgROI, pImg2, sOutImgROI, sKernelCfg);

    std::cout<<"filtered:\n";
    ROI_t viewROI_2 = {1012,0,0,width,height};
    view_image_data(pImg2, viewROI_2);

    destruct_img(&pImg1);
    destruct_img(&pImg2);
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

    Formulas_1x1_T<int> Fml;
    Fml.f1x1 = rgb_to_yuv_bt709;

    const KernelCfg_t sKernelCfg = {NULL, // 
                                    1, 1, 0, 0, // don't care for 1x1 window
                                    ZEROPADDING, // nonsense
                                    1, 1, 1, 1, // usually keep them all 1's for 1x1 window
                                    false,  // nonsense
                                    (void*)Fml.f1x1, // in use
                                    false // nonsense
                                    };

    ROI_t sInImgROI = {1012, 0, 0, width, height}; // 1012 means panel 0, 1, 2 are all chosen
    ROI_t sOutImgROI = {1012, 0, 0, width, height};

    sliding_window_1x1(pImg1, sInImgROI, pImg2, sOutImgROI, sKernelCfg);

    std::cout<<"filtered:\n";
    ROI_t viewROI_2 = {1012,0,0,width,height};
    view_image_data(pImg2, viewROI_2);

    destruct_img(&pImg1);
    destruct_img(&pImg2);
}
