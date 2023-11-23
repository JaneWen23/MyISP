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

template <typename T>
void set_kernel_arg(int* pKerArg, const StarTetrixArg_t* pArg, const CH chName){
    switch (chName){
        case Gr:
        case Y1:{
            *((T*)pKerArg) = (T)(1<<pArg->Wb);
            *((T*)pKerArg + 1) = (T)(1<<pArg->Wr);
            // the rest are dont-cares.
            break;
        }
        case Gb:
        case Y2:{
            *((T*)pKerArg) = (T)(1<<pArg->Wr);
            *((T*)pKerArg + 1) = (T)(1<<pArg->Wb);
            // the rest are dont-cares.
            break;
        }
        default:{
            break;
        }
    }
}

void set_kernel_arg_specific(const int bitDepth, int* pKerArg, const StarTetrixArg_t* pArg, const CH chName){
    if (bitDepth <= 8){
        set_kernel_arg<int8_t>(pKerArg, pArg, chName);
    }
    else if (bitDepth <= 16){
        set_kernel_arg<int16_t>(pKerArg, pArg, chName);
    }
    else if (bitDepth <= 32){
        set_kernel_arg<int>(pKerArg, pArg, chName);
    }
}

typedef struct{
    int numChs; // number of channels to process, can be 1 or 2.
    CH chNames[2];
    bool needKerArg; // some steps need arguments for kernel config.
}StarTetrixRunTimeArg_t;

const StarTetrixRunTimeArg_t find_run_time_cfg(STEP stepName){
    switch (stepName){
        case FWD_CbCr:{
            return {2, {Cb, Cr}, false};
        }
        case FWD_Y1Y2:{
            return {2, {Y1, Y2}, true};
        }
        case FWD_Delta:{
            return {1, {Delta, Delta}, false};
        }
        case FWD_Ybar:{
            return {1, {Ybar, Ybar}, false};
        }
        case BWD_Y2:{
            return {1, {Y2, Y2}, false};
        }
        case BWD_Y1:{
            return {1, {Y1, Y1}, false};
        }
        case BWD_GrGb:{
            return {2, {Gr, Gb}, true};
        }
        case BWD_BR:{
            return {2, {B, R}, false};
        }
    }
}

void star_tetrix_lifting_step(Img_t* pInImg, const ROI_t& sInImgROI, const StarTetrixArg_t* pArg, STEP stepName){
    void* pFormula = find_star_tetrix_formula_specific(pInImg->bitDepth, stepName);
    assert(pFormula != NULL);

    StarTetrixRunTimeArg_t sRunTimeArg = find_run_time_cfg(stepName);
    
    int* pKerArg = NULL;
    for (int i = 0; i < sRunTimeArg.numChs; ++i){  
        if (sRunTimeArg.needKerArg){
            pKerArg = (int*)malloc(sizeof(int) * 9); // 9, because that kernel window is 3-by-3
            set_kernel_arg_specific(pInImg->bitDepth, pKerArg, pArg, sRunTimeArg.chNames[i]);
        }
        KernelCfg_t sKernelCfg = generate_star_tetrix_kernel(pFormula, pInImg->imageFormat, sRunTimeArg.chNames[i], (uint8_t*)pKerArg);
        ROI_t sOutImgROI = generate_out_img_roi(pInImg->width, pInImg->height, pInImg->imageFormat, sRunTimeArg.chNames[i]);
        sliding_window(pInImg, sInImgROI, pInImg, sOutImgROI, sKernelCfg);
        if (sRunTimeArg.needKerArg){
            free(pKerArg);
        }
    }
}

void star_tetrix_forward(Img_t* pInImg, void* pStarTetrixArg){
    if (pInImg->sign != SIGNED){
        std::cout<<"error: Star-Tetrix transform must be a signed type, but got unsigned type. returned.\n";
        return;
    }

    StarTetrixArg_t* pArg = (StarTetrixArg_t*)pStarTetrixArg;
    ROI_t sInImgROI = {0, 0, 0, pInImg->width, pInImg->height}; // always

    star_tetrix_lifting_step(pInImg, sInImgROI, pArg, FWD_CbCr);

    star_tetrix_lifting_step(pInImg, sInImgROI, pArg, FWD_Y1Y2);

    star_tetrix_lifting_step(pInImg, sInImgROI, pArg, FWD_Delta);

    star_tetrix_lifting_step(pInImg, sInImgROI, pArg, FWD_Ybar);
}

void star_tetrix_backward(Img_t* pInImg, void* pStarTetrixArg){
    if (pInImg->sign != SIGNED){
        std::cout<<"error: Star-Tetrix transform must be a signed type, but got unsigned type. returned.\n";
        return;
    }

    StarTetrixArg_t* pArg = (StarTetrixArg_t*)pStarTetrixArg;
    ROI_t sInImgROI = {0, 0, 0, pInImg->width, pInImg->height}; // always

    star_tetrix_lifting_step(pInImg, sInImgROI, pArg, BWD_Y2);

    star_tetrix_lifting_step(pInImg, sInImgROI, pArg, BWD_Y1);

    star_tetrix_lifting_step(pInImg, sInImgROI, pArg, BWD_GrGb);

    star_tetrix_lifting_step(pInImg, sInImgROI, pArg, BWD_BR);
}

void test_star_tetrix(){
    Img_t* pInImg =(Img_t*)malloc(sizeof(Img_t));
    IMAGE_FMT imageFormat = RAW_BGGR;
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
    int Wb = 2;
    StarTetrixArg_t* pStarTetrixArg = (StarTetrixArg_t*)malloc(sizeof(StarTetrixArg_t));
    pStarTetrixArg->Wr = Wr;
    pStarTetrixArg->Wb = Wb;

    ROI_t viewROI = {0, 0, 0, pInImg->width, pInImg->height};
    std::cout<<"original:\n";
    view_image_data(pInImg, viewROI);

    star_tetrix_forward(pInImg, (void*)pStarTetrixArg);
    std::cout<<"after forward star-tetrix:\n";
    view_image_data(pInImg, viewROI);

    star_tetrix_backward(pInImg, (void*)pStarTetrixArg);
    std::cout<<"after backward star-tetrix:\n";
    view_image_data(pInImg, viewROI);

    free(pStarTetrixArg);
    destruct_img(&pInImg);
}
