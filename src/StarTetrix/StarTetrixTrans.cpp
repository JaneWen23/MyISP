#include <stdlib.h>
#include <iostream>
#include "../Math/Math.hpp"
#include "../Infra/RandImageGen.hpp"
#include "../Infra/ImageIO.hpp"
#include "StarTetrixTrans.hpp"

using namespace cv;


typedef enum{
    FWD_CbCr,
    FWD_Y1Y2,
    FWD_Delta,
    FWD_Ybar,
    BWD_Y2, // backward of Ybar
    BWD_Y1, // backward of Delta
    BWD_GrGb, // backward of Y1Y2
    BWD_BR // backward of CbCr
}STEP;

template<typename T>
void* find_star_tetrix_formula(const STEP stepName){
    Formulas_T<T> Fml;
    switch (stepName){
        case FWD_CbCr:{
            Fml.f = StarTetrix_fwd_CbCr;   
            break;
        }
        case FWD_Y1Y2:{
            Fml.f = StarTetrix_fwd_Y1Y2;
            break;
        }
        case FWD_Delta:{
            Fml.f = StarTetrix_fwd_Delta;
            break;
        }
        case FWD_Ybar:{
            Fml.f = StarTetrix_fwd_Ybar;
            break;
        }
        case BWD_Y2:{
            Fml.f = StarTetrix_bwd_Y2;
            break;
        }
        case BWD_Y1:{
            Fml.f = StarTetrix_bwd_Y1;
            break;
        }
        case BWD_GrGb:{
            Fml.f = StarTetrix_bwd_GrGb;
            break;
        }
        case BWD_BR:{
            Fml.f = StarTetrix_bwd_BR;
            break;
        }
    }
    return (void*)Fml.f;
}

void* find_star_tetrix_formula_specific(int bitDepth, const STEP stepName){
    if (bitDepth <= 8){
        return find_star_tetrix_formula<int8_t>(stepName);
    }
    else if (bitDepth <= 16){
        return find_star_tetrix_formula<int16_t>(stepName);
    }
    else if (bitDepth <= 32){
        return find_star_tetrix_formula<int>(stepName);
    }
    return NULL;
}

typedef enum{
    Cr,
    Cb,
    Y1,
    Y2,
    Delta,
    Ybar,
    Gr,
    Gb,
    R,
    B
} CH;

const int find_ch(const CH chName){
    switch (chName){
        case R:
        case Cr:{
            return 0;
        }
        case B:
        case Cb:{
            return 3;
        }
        case Gr:
        case Delta:
        case Y1:{
            return 1;
        }
        case Gb:
        case Ybar:
        case Y2:{
            return 2;
        }
    }
}

const int find_index(const IMAGE_FMT fmt, const int ch){
    switch (fmt){
        case RAW_RGGB:{
            return ch; // 0 1 2 3 --> 0 1 2 3
        }
        case RAW_GRBG:{
            return 1-(ch&1) + (ch & 2); // 0 1 2 3 --> 1 0 3 2
        }
        case RAW_GBRG:{
            return (ch+2)&3; // 0 1 2 3 --> 2 3 0 1
        }
        case RAW_BGGR:{
            return 3 - ch; // 0 1 2 3 --> 3 2 1 0
        }
        default:{
            std::cout<<"error: image format is not supported. returned.\n";
            return -1;
        }
    }
}

typedef struct{
    int rowCenter;
    int colCenter;
}STKerCenter_t;

const STKerCenter_t find_star_tetrix_kernel_center(const IMAGE_FMT fmt, const CH chName){
    // ch can only be 0, 1, 2, and 3.
    int ch = find_ch(chName);
    const STKerCenter_t centers[4] = {{1, 1}, {0, 1}, {1, 0}, {0, 0}};
    return centers[find_index(fmt, ch)];
}

typedef struct{
    int rowOffset;
    int colOffset;
}Offset_t;

const Offset_t find_star_tetrix_out_roi_offset(const IMAGE_FMT fmt, const CH chName){
    // ch can only be 0, 1, 2, and 3.
    int ch = find_ch(chName);
    const Offset_t offsets[4] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    return offsets[find_index(fmt, ch)];
}

const ROI_t generate_out_img_roi(const int imgWidth, const int imgHeight, const IMAGE_FMT fmt, const CH chName){
    Offset_t offset = find_star_tetrix_out_roi_offset(fmt, chName);
    return {0, offset.rowOffset, offset.colOffset,
            imgWidth - offset.colOffset, imgHeight - offset.rowOffset};
}

const KernelCfg_t generate_star_tetrix_kernel(void* pFormula, const IMAGE_FMT fmt, const CH chName, uint8_t* kerArg = NULL){
    STKerCenter_t kerCenter = find_star_tetrix_kernel_center(fmt, chName);
    return {kerArg,
            3, 3,
            kerCenter.rowCenter, kerCenter.colCenter,
            MIRROR,
            2, 2, 2, 2,
            false, pFormula, false};
}

void star_tetrix_forward_step_1(Img_t* pInImg, const ROI_t& sInImgROI){
    ROI_t sOutImgROI_Cb = generate_out_img_roi(pInImg->width, pInImg->height, pInImg->imageFormat, Cb);
    ROI_t sOutImgROI_Cr = generate_out_img_roi(pInImg->width, pInImg->height, pInImg->imageFormat, Cr);

    void* pFormula = find_star_tetrix_formula_specific(pInImg->bitDepth, FWD_CbCr);
    assert(pFormula != NULL);

    KernelCfg_t sKernelCfg_Cb = generate_star_tetrix_kernel(pFormula, pInImg->imageFormat, Cb);
    KernelCfg_t sKernelCfg_Cr = generate_star_tetrix_kernel(pFormula, pInImg->imageFormat, Cr);

    // the following two functions can be processed in parallel, that's why they are in the same step
    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Cb, sKernelCfg_Cb);
    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Cr, sKernelCfg_Cr);
}

template <typename T>
void set_kernel_arg_Y1Y2(int* pKerArg_Y1, int* pKerArg_Y2, const StarTetrixArg_t* pArg){
    *((T*)pKerArg_Y1) = (T)(1<<pArg->Wb);
    *((T*)pKerArg_Y1 + 1) = (T)(1<<pArg->Wr);
    *((T*)pKerArg_Y2) = (T)(1<<pArg->Wr);
    *((T*)pKerArg_Y2 + 1) = (T)(1<<pArg->Wb);
    // the rest are dont-cares.
}

void set_kernel_arg_Y1Y2_specific(const int bitDepth, int* pKerArg_Y1, int* pKerArg_Y2, const StarTetrixArg_t* pArg){
    if (bitDepth <= 8){
        set_kernel_arg_Y1Y2<int8_t>(pKerArg_Y1, pKerArg_Y2, pArg);
    }
    else if (bitDepth <= 16){
        set_kernel_arg_Y1Y2<int16_t>(pKerArg_Y1, pKerArg_Y2, pArg);
    }
    else if (bitDepth <= 32){
        set_kernel_arg_Y1Y2<int>(pKerArg_Y1, pKerArg_Y2, pArg);
    }
}

void star_tetrix_forward_step_2(Img_t* pInImg, const ROI_t& sInImgROI, const StarTetrixArg_t* pArg){
    ROI_t sOutImgROI_Y1 = generate_out_img_roi(pInImg->width, pInImg->height, pInImg->imageFormat, Y1);
    ROI_t sOutImgROI_Y2 = generate_out_img_roi(pInImg->width, pInImg->height, pInImg->imageFormat, Y2);

    void* pFormula = find_star_tetrix_formula_specific(pInImg->bitDepth, FWD_Y1Y2);
    assert(pFormula != NULL);

    int* pKerArg_Y1 = (int*)malloc(sizeof(int) * 9); // 9, because that kernel window is 3-by-3
    int* pKerArg_Y2 = (int*)malloc(sizeof(int) * 9); // 9, because that kernel window is 3-by-3
    set_kernel_arg_Y1Y2_specific(pInImg->bitDepth, pKerArg_Y1, pKerArg_Y2, pArg);

    KernelCfg_t sKernelCfg_Y1 = generate_star_tetrix_kernel(pFormula, pInImg->imageFormat, Y1, (uint8_t*)pKerArg_Y1);
    KernelCfg_t sKernelCfg_Y2 = generate_star_tetrix_kernel(pFormula, pInImg->imageFormat, Y2, (uint8_t*)pKerArg_Y2);

    // the following two functions can be processed in parallel, that's why they are in the same step
    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Y1, sKernelCfg_Y1);
    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Y2, sKernelCfg_Y2);

    free(pKerArg_Y1);
    free(pKerArg_Y2);
}

void star_tetrix_forward_step_3(Img_t* pInImg, const ROI_t& sInImgROI){
    ROI_t sOutImgROI_Delta = generate_out_img_roi(pInImg->width, pInImg->height, pInImg->imageFormat, Delta);

    void* pFormula = find_star_tetrix_formula_specific(pInImg->bitDepth, FWD_Delta);
    assert(pFormula != NULL);

    KernelCfg_t sKernelCfg_Delta = generate_star_tetrix_kernel(pFormula, pInImg->imageFormat, Delta);

    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Delta, sKernelCfg_Delta);
}

void star_tetrix_forward_step_4(Img_t* pInImg, const ROI_t& sInImgROI){
    ROI_t sOutImgROI_Ybar = generate_out_img_roi(pInImg->width, pInImg->height, pInImg->imageFormat, Ybar);

    void* pFormula = find_star_tetrix_formula_specific(pInImg->bitDepth, FWD_Ybar);
    assert(pFormula != NULL);

    KernelCfg_t sKernelCfg_Ybar = generate_star_tetrix_kernel(pFormula, pInImg->imageFormat, Ybar);

    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Ybar, sKernelCfg_Ybar);
}


void star_tetrix_forward(Img_t* pInImg, void* pStarTetrixArg){
    if (pInImg->sign != SIGNED){
        std::cout<<"error: Star-Tetrix transform must be a signed type, but got unsigned type. returned.\n";
        return;
    }

    StarTetrixArg_t* pArg = (StarTetrixArg_t*)pStarTetrixArg;
    ROI_t sInImgROI = {0, 0, 0, pInImg->width, pInImg->height}; // always


    ROI_t viewROI = {0, 0, 0, pInImg->width, pInImg->height};

    star_tetrix_forward_step_1(pInImg, sInImgROI);

    star_tetrix_forward_step_2(pInImg, sInImgROI, pArg);
    // std::cout<<"filtered, Y1Y2:\n";
    // view_image_data(pInImg, viewROI );

    star_tetrix_forward_step_3(pInImg, sInImgROI);
    // std::cout<<"filtered, Delta:\n";
    // view_image_data(pInImg, viewROI );

    star_tetrix_forward_step_4(pInImg, sInImgROI);
    std::cout<<"filtered, Ybar:\n";
    view_image_data(pInImg, viewROI );
}

void test_star_tetrix_forward(){
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

    int Wr = 1;
    int Wb = 1;
    StarTetrixArg_t* pStarTetrixArg = (StarTetrixArg_t*)malloc(sizeof(StarTetrixArg_t));
    pStarTetrixArg->Wr = Wr;
    pStarTetrixArg->Wb = Wb;

    star_tetrix_forward(pInImg, (void*)pStarTetrixArg);

    free(pStarTetrixArg);
    destruct_img(&pInImg);
}

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

    int Wr = 1;
    int Wb = 1;
    StarTetrixArg_t* pArg = (StarTetrixArg_t*)malloc(sizeof(StarTetrixArg_t));
    pArg->Wr = Wr;
    pArg->Wb = Wb;

    ROI_t sInImgROI = {0, 0, 0, pInImg->width, pInImg->height}; // always
    ROI_t sOutImgROI_Cb = {0, 1, 1, pInImg->width-1, pInImg->height-1}; // determined by bayer pattern, irrelevant to fwd/bwd // original B
    ROI_t sOutImgROI_Cr = {0, 0, 0, pInImg->width, pInImg->height}; // original R
    ROI_t sOutImgROI_Y1 = {0, 0, 1, pInImg->width-1, pInImg->height}; // original Gr
    ROI_t sOutImgROI_Y2 = {0, 1, 0, pInImg->width, pInImg->height-1}; // original Gb
    ROI_t sOutImgROI_Delta = sOutImgROI_Y1;
    ROI_t sOutImgROI_Ybar = sOutImgROI_Y2;

    Formulas_T<int16_t> Fml_step1;
    Fml_step1.f = StarTetrix_fwd_CbCr; // determined by step and fwd/bwd, naturally determined in a certain function
    const KernelCfg_t sKernelCfg_Cb = {
        NULL, 3, 3, 0, 0, MIRROR, 2, 2, 2, 2, false, (void*)Fml_step1.f, false};// determined by bayer pattern and formula
    const KernelCfg_t sKernelCfg_Cr = {
        NULL, 3, 3, 1, 1, MIRROR, 2, 2, 2, 2, false, (void*)Fml_step1.f, false};// determined by bayer pattern and formula

    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Cb, sKernelCfg_Cb);
    // std::cout<<"filtered, Cb:\n";
    // view_image_data(pInImg, viewROI );
    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Cr, sKernelCfg_Cr);
    // std::cout<<"filtered, Cr:\n";
    // view_image_data(pInImg, viewROI );



    Formulas_T<int16_t> Fml_step2;
    Fml_step2.f = StarTetrix_fwd_Y1Y2;
    int16_t pKerArg_Y1[9] = {(int16_t)(1<<pArg->Wb), (int16_t)(1<<pArg->Wr),
                            0, 0, 0, 0, 0, 0, 0}; // only the first two are in use, but we have to define 9 values because kernel window is 3-by-3
    int16_t pKerArg_Y2[9] = {(int16_t)(1<<pArg->Wr), (int16_t)(1<<pArg->Wb),
                            0, 0, 0, 0, 0, 0, 0}; // only the first two are in use, but we have to define 9 values because kernel window is 3-by-3

    const KernelCfg_t sKernelCfg_Y1 = {
        (uint8_t*)pKerArg_Y1, 3, 3, 0, 1, MIRROR, 2, 2, 2, 2, false, (void*)Fml_step2.f, false}; // Gr
    const KernelCfg_t sKernelCfg_Y2 = {
        (uint8_t*)pKerArg_Y2, 3, 3, 1, 0, MIRROR, 2, 2, 2, 2, false, (void*)Fml_step2.f, false}; // Gb
    
    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Y1, sKernelCfg_Y1);
    // std::cout<<"filtered, Y1:\n";
    // view_image_data(pInImg, viewROI );
    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Y2, sKernelCfg_Y2);
    // std::cout<<"filtered, Y2:\n";
    // view_image_data(pInImg, viewROI );



    Formulas_T<int16_t> Fml_step3;
    Fml_step3.f = StarTetrix_fwd_Delta;
    const KernelCfg_t sKernelCfg_Delta = {
        NULL, 3, 3, 0, 1, MIRROR, 2, 2, 2, 2, false, (void*)Fml_step3.f, false}; // Gr Y1
    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Delta, sKernelCfg_Delta);
    // std::cout<<"filtered, Delta:\n";
    // view_image_data(pInImg, viewROI );


    Formulas_T<int16_t> Fml_step4;
    Fml_step4.f = StarTetrix_fwd_Ybar;
    const KernelCfg_t sKernelCfg_Ybar = {
        NULL, 3, 3, 1, 0, MIRROR, 2, 2, 2, 2, false, (void*)Fml_step4.f, false}; // Gb Y2
    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Ybar, sKernelCfg_Ybar);
    std::cout<<"filtered, Ybar:\n";
    view_image_data(pInImg, viewROI );


//========================================

    Formulas_T<int16_t> Fml_step4_back;
    Fml_step4_back.f = StarTetrix_bwd_Y2;
    const KernelCfg_t sKernelCfg_Ybar_bwd = {
        NULL, 3, 3, 1, 0, MIRROR, 2, 2, 2, 2, false, (void*)Fml_step4_back.f, false}; // Gb Y2
    //ROI_t sOutImgROI_Ybar = {0, 1, 0, pInImg->width, pInImg->height-1}; // Gb Y2
    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Ybar, sKernelCfg_Ybar_bwd);
    // std::cout<<"filtered, Ybar back (Delta):\n";
    // view_image_data(pInImg, viewROI );

    Formulas_T<int16_t> Fml_step3_back;
    Fml_step3_back.f = StarTetrix_bwd_Y1;
    const KernelCfg_t sKernelCfg_Delta_bwd = {
        NULL, 3, 3, 0, 1, MIRROR, 2, 2, 2, 2, false, (void*)Fml_step3_back.f, false}; // Gr Y1
    //ROI_t sOutImgROI_Delta = {0, 0, 1, pInImg->width-1, pInImg->height}; // Gr Y1
    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Delta, sKernelCfg_Delta_bwd);
    // std::cout<<"filtered, Delta back (Y2):\n";
    // view_image_data(pInImg, viewROI );

    Formulas_T<int16_t> Fml_step2_back;
    Fml_step2_back.f = StarTetrix_bwd_GrGb;
    //int16_t pKerArg_Y1[9] = {(int16_t)(1<<Wb), (int16_t)(1<<Wr), 0, 0, 0, 0, 0, 0, 0}; // only the first two are in use, but we have to define 9 values because kernel window is 3-by-3
    const KernelCfg_t sKernelCfg_Y1_bwd = {
        (uint8_t*)pKerArg_Y1, 3, 3, 0, 1, MIRROR, 2, 2, 2, 2, false, (void*)Fml_step2_back.f, false}; // Gr
    //ROI_t sOutImgROI_Y1 = {0, 0, 1, pInImg->width-1, pInImg->height}; // Gr
    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Y1, sKernelCfg_Y1_bwd);
    // std::cout<<"filtered, Y1 back (B):\n";
    // view_image_data(pInImg, viewROI );

    //int16_t pKerArg_Y2[9] = {(int16_t)(1<<Wr), (int16_t)(1<<Wb), 0, 0, 0, 0, 0, 0, 0}; // only the first two are in use, but we have to define 9 values because kernel window is 3-by-3
    const KernelCfg_t sKernelCfg_Y2_bwd = {
        (uint8_t*)pKerArg_Y2, 3, 3, 1, 0, MIRROR, 2, 2, 2, 2, false, (void*)Fml_step2_back.f, false}; // Gb
    //ROI_t sOutImgROI_Y2 = {0, 1, 0, pInImg->width, pInImg->height-1}; // Gb
    sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI_Y2, sKernelCfg_Y2_bwd);
    // std::cout<<"filtered, Y2 back (Cr):\n";
    // view_image_data(pInImg, viewROI );

    Formulas_T<int16_t> Fml_step1_back;
    Fml_step1_back.f = StarTetrix_bwd_BR;
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