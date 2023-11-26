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
    // return value is a pointer to function, converted to void*.
    switch (stepName){
        case FWD_CbCr:{
            return (void*)StarTetrix_fwd_CbCr<T>;   
        }
        case FWD_Y1Y2:{
            return (void*)StarTetrix_fwd_Y1Y2<T>;
        }
        case FWD_Delta:{
            return (void*)StarTetrix_fwd_Delta<T>;
        }
        case FWD_Ybar:{
            return (void*)StarTetrix_fwd_Ybar<T>;
        }
        case BWD_Y2:{
            return (void*)StarTetrix_bwd_Y2<T>;
        }
        case BWD_Y1:{
            return (void*)StarTetrix_bwd_Y1<T>;
        }
        case BWD_GrGb:{
            return (void*)StarTetrix_bwd_GrGb<T>;
        }
        case BWD_BR:{
            return (void*)StarTetrix_bwd_BR<T>;
        }
    }
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
    const STKerCenter_t centers[4] = {{1, 1}, {0, 1}, {1, 0}, {0, 0}};
    int ch = find_ch(chName); // ch can only be 0, 1, 2, and 3.
    return centers[find_index(fmt, ch)];
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

typedef struct{
    int rowOffset;
    int colOffset;
}Offset_t;

const Offset_t find_star_tetrix_roi_offset(const IMAGE_FMT fmt, const CH chName){
    const Offset_t offsets[4] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    int ch = find_ch(chName); // ch can only be 0, 1, 2, and 3.
    return offsets[find_index(fmt, ch)];
}

const ROI_t generate_img_roi(const int imgWidth, const int imgHeight, const IMAGE_FMT fmt, const CH chName){
    Offset_t offset = find_star_tetrix_roi_offset(fmt, chName);
    return {0, offset.rowOffset, offset.colOffset,
            imgWidth - offset.colOffset, imgHeight - offset.rowOffset};
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

void star_tetrix_lifting_step(Img_t* pImg, const ROI_t& sInImgROI, const StarTetrixArg_t* pArg, STEP stepName){
    void* pFormula = find_star_tetrix_formula_specific(pImg->bitDepth, stepName);
    assert(pFormula != NULL);

    StarTetrixRunTimeArg_t sRunTimeArg = find_run_time_cfg(stepName);
    
    int* pKerArg = NULL;
    for (int i = 0; i < sRunTimeArg.numChs; ++i){  
        if (sRunTimeArg.needKerArg){
            pKerArg = (int*)malloc(sizeof(int) * 9); // 9, because kernel window is 3-by-3; but not all are used.
            set_kernel_arg_specific(pImg->bitDepth, pKerArg, pArg, sRunTimeArg.chNames[i]);
        }
        KernelCfg_t sKernelCfg = generate_star_tetrix_kernel(pFormula, pImg->imageFormat, sRunTimeArg.chNames[i], (uint8_t*)pKerArg);
        ROI_t sOutImgROI = generate_img_roi(pImg->width, pImg->height, pImg->imageFormat, sRunTimeArg.chNames[i]);
        sliding_window(pImg, sInImgROI, pImg, sOutImgROI, sKernelCfg);
        if (sRunTimeArg.needKerArg){
            free(pKerArg);
        }
    }
}

IMAGE_FMT find_image_fmt(IMAGE_FMT srcFmt){
    switch (srcFmt){
        case RAW_RGGB:{
            return Y_C_C_D_RGGB;
        }
        case RAW_GRBG:{
            return Y_C_C_D_GRBG;
        }
        case RAW_GBRG:{
            return Y_C_C_D_GBRG;
        }
        case RAW_BGGR:{
            return Y_C_C_D_BGGR;
        }
        case Y_C_C_D_RGGB:{
            return RAW_RGGB;
        }
        case Y_C_C_D_GRBG:{
            return RAW_GRBG;
        }
        case Y_C_C_D_GBRG:{
            return RAW_GBRG;
        }
        case Y_C_C_D_BGGR:{
            return RAW_BGGR;
        }
        default:{
            return UNKOWN;
        }
    }
}
void convert_1p_to_4p(Img_t* pSrcImg, Img_t* pDstImg){
    assert((pSrcImg->width & 1) == 0);
    assert((pSrcImg->height & 1) == 0);
    construct_img(pDstImg, 
                  find_image_fmt(pSrcImg->imageFormat),
                  pSrcImg->width >> 1,
                  pSrcImg->height >> 1,
                  SIGNED,
                  pSrcImg->bitDepth,
                  pSrcImg->alignment,
                  true);

    KernelCfg_t sKerCfg = {NULL, // nonsense
                           1, 1, // nonsense
                           0, 0, // nonsense
                           MIRROR, // nonsense
                           2, 2, 1, 1, // in use
                           false, NULL, false}; // nonsense
    
    CH tmpCh[4] = {R, Gr, Gb, B};
    int tmpId[4] = {2, 3, 0, 1};
    for (int i = 0; i < 4; ++i){
        ROI_t sSrcImgROI = generate_img_roi(pSrcImg->width, pSrcImg->height, pSrcImg->imageFormat, tmpCh[i]);
        ROI_t sDstImgROI = {tmpId[i], 0, 0, pDstImg->width, pDstImg->height};
        sliding_window_1x1(pSrcImg, sSrcImgROI, pDstImg, sDstImgROI, sKerCfg);
    }
}

void convert_4p_to_1p(Img_t* pSrcImg, Img_t* pDstImg){
    construct_img(pDstImg,
                  find_image_fmt(pSrcImg->imageFormat),
                  pSrcImg->width << 1,
                  pSrcImg->height << 1,
                  SIGNED,
                  pSrcImg->bitDepth,
                  pSrcImg->alignment,
                  true);

    KernelCfg_t sKerCfg = {NULL, // nonsense
                           1, 1, // nonsense
                           0, 0, // nonsense
                           MIRROR, // nonsense
                           1, 1, 2, 2, // in use
                           false, NULL, false}; // nonsense
    
    CH tmpCh[4] = {R, Gr, Gb, B};
    int tmpId[4] = {2, 3, 0, 1};
    for (int i = 0; i < 4; ++i){
        ROI_t sSrcImgROI = {tmpId[i], 0, 0, pSrcImg->width, pSrcImg->height};
        ROI_t sDstImgROI = generate_img_roi(pDstImg->width, pDstImg->height, pDstImg->imageFormat, tmpCh[i]);
        sliding_window_1x1(pSrcImg, sSrcImgROI, pDstImg, sDstImgROI, sKerCfg);
    }
}

IMG_RTN_CODE star_tetrix_forward(Img_t* pInImg, Img_t* pOutImg, void* pStarTetrixArg){
    if (pInImg->sign != SIGNED){
        std::cout<<"error: Star-Tetrix transform must be a signed type, but got unsigned type. returned.\n";
        return INVALID_INPUT;
    }
    if( ((pInImg->width & 1) == 1) || ((pInImg->height & 1) == 1)){
        std::cout<<"error: raw image width and height must be even, but got odd number. returned.\n";
        return INVALID_INPUT;
    }
    // duplicate the input img and process the duplicated version:
    Img_t* pTmpImg =(Img_t*)malloc(sizeof(Img_t));
    duplicate_img(pInImg, pTmpImg);

    StarTetrixArg_t* pArg = (StarTetrixArg_t*)pStarTetrixArg;
    ROI_t sInImgROI = {0, 0, 0, pTmpImg->width, pTmpImg->height}; // always

    star_tetrix_lifting_step(pTmpImg, sInImgROI, pArg, FWD_CbCr);

    star_tetrix_lifting_step(pTmpImg, sInImgROI, pArg, FWD_Y1Y2);

    star_tetrix_lifting_step(pTmpImg, sInImgROI, pArg, FWD_Delta);

    star_tetrix_lifting_step(pTmpImg, sInImgROI, pArg, FWD_Ybar);

    convert_1p_to_4p(pTmpImg, pOutImg);
    free(pTmpImg);
    return SUCCEED;
}

IMG_RTN_CODE star_tetrix_backward(Img_t* pInImg, Img_t* pOutImg, void* pStarTetrixArg){
    if (pInImg->sign != SIGNED){
        std::cout<<"error: Star-Tetrix transform must be a signed type, but got unsigned type. returned.\n";
        return INVALID_INPUT;
    }

    convert_4p_to_1p(pInImg,  pOutImg);

    StarTetrixArg_t* pArg = (StarTetrixArg_t*)pStarTetrixArg;
    ROI_t sOutImgROI = {0, 0, 0, pOutImg->width, pOutImg->height}; // always

    star_tetrix_lifting_step(pOutImg, sOutImgROI, pArg, BWD_Y2);

    star_tetrix_lifting_step(pOutImg, sOutImgROI, pArg, BWD_Y1);

    star_tetrix_lifting_step(pOutImg, sOutImgROI, pArg, BWD_GrGb);

    star_tetrix_lifting_step(pOutImg, sOutImgROI, pArg, BWD_BR);

    return SUCCEED;
}

void test_star_tetrix(){
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

    int Wr = 1;
    int Wb = 2;
    StarTetrixArg_t* pStarTetrixArg = (StarTetrixArg_t*)malloc(sizeof(StarTetrixArg_t));
    pStarTetrixArg->Wr = Wr;
    pStarTetrixArg->Wb = Wb;

    ROI_t viewROI = {0, 0, 0, pInImg->width, pInImg->height};
    std::cout<<"original:\n";
    view_image_data(pInImg, viewROI);

    Img_t* pOutImg =(Img_t*)malloc(sizeof(Img_t));
    star_tetrix_forward(pInImg, pOutImg, (void*)pStarTetrixArg);
    std::cout<<"after forward star-tetrix, Ybar:\n";
    ROI_t viewROI_out_0 = {0, 0, 0, pInImg->width>>1, pInImg->height>>1};
    view_image_data(pOutImg, viewROI_out_0);
    std::cout<<"after forward star-tetrix, Cb:\n";
    ROI_t viewROI_out_1 = {1, 0, 0, pInImg->width>>1, pInImg->height>>1};
    view_image_data(pOutImg, viewROI_out_1);
    std::cout<<"after forward star-tetrix, Cr:\n";
    ROI_t viewROI_out_2 = {2, 0, 0, pInImg->width>>1, pInImg->height>>1};
    view_image_data(pOutImg, viewROI_out_2);
    std::cout<<"after forward star-tetrix, Delta:\n";
    ROI_t viewROI_out_3 = {3, 0, 0, pInImg->width>>1, pInImg->height>>1};
    view_image_data(pOutImg, viewROI_out_3);

    Img_t* pBackOutImg =(Img_t*)malloc(sizeof(Img_t));   
    star_tetrix_backward(pOutImg, pBackOutImg, (void*)pStarTetrixArg);
    std::cout<<"after backward star-tetrix:\n";
    view_image_data(pBackOutImg, viewROI);

    free(pStarTetrixArg);
    destruct_img(&pInImg);
    destruct_img(&pOutImg);
    destruct_img(&pBackOutImg);
}

void demo_star_tetrix(){
    Img_t* pInImg = NULL;
    pInImg =(Img_t*)malloc(sizeof(Img_t));
    IMAGE_FMT imageFormat = RAW_RGGB;
    int width = 4256; // 4240
    int height = 2848; // 2832
    int bitDepth = 16;
    int alignment = 1; //32
    
    read_raw_to_img_t("../data/rawData.raw",
                      pInImg,
                      imageFormat,
                      width,
                      height,
                      bitDepth,
                      alignment);
    pInImg->sign = SIGNED; // to make star-tetrix happy
    Img_t* pOutImg =(Img_t*)malloc(sizeof(Img_t));
    StarTetrixArg_t* pStarTetrixArg = (StarTetrixArg_t*)malloc(sizeof(StarTetrixArg_t));
    pStarTetrixArg->Wr = 1;
    pStarTetrixArg->Wb = 2;
    star_tetrix_forward(pInImg, pOutImg, (void*)pStarTetrixArg);  

    Mat pCvImageArray[4];
    convert_img_t_to_cv_mat(pCvImageArray, pOutImg);

    imwrite("star_tetrix_Ybar.png", pCvImageArray[0]);
    imwrite("star_tetrix_Cb.png", pCvImageArray[1]);
    imwrite("star_tetrix_Cr.png", pCvImageArray[2]);
    imwrite("star_tetrix_Delta.png", pCvImageArray[3]);
}