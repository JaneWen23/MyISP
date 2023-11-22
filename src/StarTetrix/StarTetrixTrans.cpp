#include <stdlib.h>
#include <iostream>
#include "../Math/Math.hpp"
#include "../Infra/ImgDef.hpp"
#include "../Filter/SlidingWindow.hpp"
#include "../Infra/RandImageGen.hpp"
#include "../Infra/ImageIO.hpp"
#include "StarTetrixTrans.hpp"

using namespace cv;

void test_star_tetrix(){
   Img_t* pInImg =(Img_t*)malloc(sizeof(Img_t));
    IMAGE_FMT imageFormat = RAW_RGGB;
    int width = 10;
    int height = 10;
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

    std::cout<<"original:\n";
    ROI_t viewROI = {0, 0, 0, pInImg->width, pInImg->height};
    view_image_data(pInImg, viewROI );

    ROI_t sInImgROI = {0, 0, 0, pInImg->width, pInImg->height};

    Formulas_T<int16_t> Fml_step1;
    Fml_step1.f = StarTetrix_CbCr_fwd;
    const KernelCfg_t sKernelCfg_Cb = {
        NULL, 3, 3, 0, 0, MIRROR, 2, 2, 2, 2, false, (void*)Fml_step1.f, false};
    ROI_t sOutImgROI_Cb = {0, 1, 1, pInImg->width-1, pInImg->height-1};
    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Cb, sKernelCfg_Cb);
    // std::cout<<"filtered, Cb:\n";
    // view_image_data(pInImg, viewROI );

    const KernelCfg_t sKernelCfg_Cr = {
        NULL, 3, 3, 1, 1, MIRROR, 2, 2, 2, 2, false, (void*)Fml_step1.f, false};
    ROI_t sOutImgROI_Cr = {0, 0, 0, pInImg->width, pInImg->height};
    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Cr, sKernelCfg_Cr);
    // std::cout<<"filtered, Cr:\n";
    // view_image_data(pInImg, viewROI );

    int Wr = 6;
    int Wb = 5;
    Formulas_T<int16_t> Fml_step2;
    Fml_step2.f = StarTetrix_Y1Y2_fwd;
    int16_t kerArg_Y1[9] = {(int16_t)(1<<Wb), (int16_t)(1<<Wr), 0, 0, 0, 0, 0, 0, 0}; // only the first two are in use, but we have to define 9 values because kernel window is 3-by-3
    const KernelCfg_t sKernelCfg_Y1 = {
        (uint8_t*)kerArg_Y1, 3, 3, 0, 1, MIRROR, 2, 2, 2, 2, false, (void*)Fml_step2.f, false}; // Gr
    ROI_t sOutImgROI_Y1 = {0, 0, 1, pInImg->width-1, pInImg->height}; // Gr
    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Y1, sKernelCfg_Y1);
    // std::cout<<"filtered, Y1:\n";
    // view_image_data(pInImg, viewROI );

    int16_t kerArg_Y2[9] = {(int16_t)(1<<Wr), (int16_t)(1<<Wb), 0, 0, 0, 0, 0, 0, 0}; // only the first two are in use, but we have to define 9 values because kernel window is 3-by-3
    const KernelCfg_t sKernelCfg_Y2 = {
        (uint8_t*)kerArg_Y2, 3, 3, 1, 0, MIRROR, 2, 2, 2, 2, false, (void*)Fml_step2.f, false}; // Gb
    ROI_t sOutImgROI_Y2 = {0, 1, 0, pInImg->width, pInImg->height-1}; // Gb
    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Y2, sKernelCfg_Y2);
    // std::cout<<"filtered, Y2:\n";
    // view_image_data(pInImg, viewROI );

    Formulas_T<int16_t> Fml_step3;
    Fml_step3.f = StarTetrix_Delta_fwd;
    const KernelCfg_t sKernelCfg_Delta = {
        NULL, 3, 3, 0, 1, MIRROR, 2, 2, 2, 2, false, (void*)Fml_step3.f, false}; // Gr Y1
    ROI_t sOutImgROI_Delta = {0, 0, 1, pInImg->width-1, pInImg->height}; // Gr Y1
    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Delta, sKernelCfg_Delta);
    // std::cout<<"filtered, Delta:\n";
    // view_image_data(pInImg, viewROI );

    Formulas_T<int16_t> Fml_step4;
    Fml_step4.f = StarTetrix_Ybar_fwd;
    const KernelCfg_t sKernelCfg_Ybar = {
        NULL, 3, 3, 1, 0, MIRROR, 2, 2, 2, 2, false, (void*)Fml_step4.f, false}; // Gb Y2
    ROI_t sOutImgROI_Ybar = {0, 1, 0, pInImg->width, pInImg->height-1}; // Gb Y2
    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Ybar, sKernelCfg_Ybar);
    std::cout<<"filtered, Ybar:\n";
    view_image_data(pInImg, viewROI );

    Formulas_T<int16_t> Fml_step4_back;
    Fml_step4_back.f = StarTetrix_Ybar_bwd;
    const KernelCfg_t sKernelCfg_Ybar_bwd = {
        NULL, 3, 3, 1, 0, MIRROR, 2, 2, 2, 2, false, (void*)Fml_step4_back.f, false}; // Gb Y2
    //ROI_t sOutImgROI_Ybar = {0, 1, 0, pInImg->width, pInImg->height-1}; // Gb Y2
    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Ybar, sKernelCfg_Ybar_bwd);
    // std::cout<<"filtered, Ybar back (Delta):\n";
    // view_image_data(pInImg, viewROI );

    Formulas_T<int16_t> Fml_step3_back;
    Fml_step3_back.f = StarTetrix_Delta_bwd;
    const KernelCfg_t sKernelCfg_Delta_bwd = {
        NULL, 3, 3, 0, 1, MIRROR, 2, 2, 2, 2, false, (void*)Fml_step3_back.f, false}; // Gr Y1
    //ROI_t sOutImgROI_Delta = {0, 0, 1, pInImg->width-1, pInImg->height}; // Gr Y1
    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Delta, sKernelCfg_Delta_bwd);
    // std::cout<<"filtered, Delta back (Y2):\n";
    // view_image_data(pInImg, viewROI );

    Formulas_T<int16_t> Fml_step2_back;
    Fml_step2_back.f = StarTetrix_Y1Y2_bwd;
    //int16_t kerArg_Y1[9] = {(int16_t)(1<<Wb), (int16_t)(1<<Wr), 0, 0, 0, 0, 0, 0, 0}; // only the first two are in use, but we have to define 9 values because kernel window is 3-by-3
    const KernelCfg_t sKernelCfg_Y1_bwd = {
        (uint8_t*)kerArg_Y1, 3, 3, 0, 1, MIRROR, 2, 2, 2, 2, false, (void*)Fml_step2_back.f, false}; // Gr
    //ROI_t sOutImgROI_Y1 = {0, 0, 1, pInImg->width-1, pInImg->height}; // Gr
    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Y1, sKernelCfg_Y1_bwd);
    // std::cout<<"filtered, Y1 back (B):\n";
    // view_image_data(pInImg, viewROI );

    //int16_t kerArg_Y2[9] = {(int16_t)(1<<Wr), (int16_t)(1<<Wb), 0, 0, 0, 0, 0, 0, 0}; // only the first two are in use, but we have to define 9 values because kernel window is 3-by-3
    const KernelCfg_t sKernelCfg_Y2_bwd = {
        (uint8_t*)kerArg_Y2, 3, 3, 1, 0, MIRROR, 2, 2, 2, 2, false, (void*)Fml_step2_back.f, false}; // Gb
    //ROI_t sOutImgROI_Y2 = {0, 1, 0, pInImg->width, pInImg->height-1}; // Gb
    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Y2, sKernelCfg_Y2_bwd);
    // std::cout<<"filtered, Y2 back (Cr):\n";
    // view_image_data(pInImg, viewROI );

    Formulas_T<int16_t> Fml_step1_back;
    Fml_step1_back.f = StarTetrix_CbCr_bwd;
    const KernelCfg_t sKernelCfg_Cb_bwd = {
        NULL, 3, 3, 0, 0, MIRROR, 2, 2, 2, 2, false, (void*)Fml_step1_back.f, false};
    //ROI_t sOutImgROI_Cb = {0, 1, 1, pInImg->width-1, pInImg->height-1};
    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Cb, sKernelCfg_Cb_bwd);
    // std::cout<<"filtered, Cb back:\n";
    // view_image_data(pInImg, viewROI );

    const KernelCfg_t sKernelCfg_Cr_bwd = {
        NULL, 3, 3, 1, 1, MIRROR, 2, 2, 2, 2, false, (void*)Fml_step1_back.f, false};
    //ROI_t sOutImgROI_Cr = {0, 0, 0, pInImg->width, pInImg->height};
    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Cr, sKernelCfg_Cr_bwd);
    std::cout<<"filtered, Cr back (original):\n";
    view_image_data(pInImg, viewROI );
}