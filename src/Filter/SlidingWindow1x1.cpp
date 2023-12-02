#include "SlidingWindow.hpp"
#include "../Math/Math.hpp"

const int iter_map1(const int i){
    return i;
}

const int iter_map2(const int i){
    return i >> 1;
}

const int iter_map3(const int i){
    return 1;
}

const int iter_map4(const int i){
    return i & 1;
}

typedef const int (*FMAP)(const int);

const int find_output_ch_user(int* cList, const int panelId, const int ii, const int jj){
    cList[0] = panelId;
    return 1; // this is "outChNum", the length of cList that in use.
}

const int find_output_ch_rgb(int* cList, const int panelId, const int ii=0, const int jj=0){
    cList[0] = 0;
    cList[1] = 1;
    cList[2] = 2;
    return 3; // this is "outChNum", the length of cList that in use.
}


const int find_output_ch_yuv420(int* cList, const int panelId, const int ii, const int jj){
    cList[0] = 0;
    cList[1] = 1 + (ii & 1);
    return 2 - (jj & 1); // this is "outChNum", the length of cList that in use.
}

typedef const int (*FCH)(int*, const int, const int, const int);

void set_in_img_iter_manipulation(const IMAGE_FMT fmt, FMAP* f_hori, FMAP* f_vert){
        switch (fmt){
        case RGB:{
            f_hori[0] = f_hori[1] = f_hori[2] = iter_map3;
            f_vert[0] = f_vert[1] = f_vert[2] = iter_map1;
            break;
        }
        case YUV420:{
            f_vert[0] = iter_map1;
            f_vert[1] = f_vert[2] = iter_map2;
            f_hori[0] = iter_map3;
            f_hori[1] = f_hori[2] = iter_map4;
            break;
        }
        default:{
            std::cout<<"error: input image format not supported. exited.\n";
            exit(1);
        }
    }
}


void set_out_img_iter_manipulation(const IMAGE_FMT fmt, FMAP* g_hori, FMAP* g_vert){
    switch (fmt){
        case RGB:{
            g_hori[0] = g_hori[1] = g_hori[2] = iter_map3;
            g_vert[0] = g_vert[1] = g_vert[2] = iter_map1;
            break;
        }
        case YUV420:{
            g_vert[0] = iter_map1;
            g_vert[1] = g_vert[2] = iter_map2;
            g_hori[0] = iter_map3;
            g_hori[1] = g_hori[2] = iter_map4;
            break;
        }
        default:{
            std::cout<<"error: output image format not supported. exited.\n";
            exit(1);
        }
    }
}


void set_output_ch_manipulation(const int panelId, const IMAGE_FMT fmt, FCH f_ch){
    if (panelId < MAX_NUM_P){ // single panel is chosen
        f_ch = find_output_ch_user;
    } 
    else if (panelId == 1012){ // panel 0, 1, and 2
        if (fmt == RGB){
            f_ch = find_output_ch_rgb;
        }
        else if (fmt == YUV420){
            f_ch = find_output_ch_yuv420;
        }
    }
    else{
        std::cout<<"error: output image ROI panelId not supported. exited.\n";
        exit(1);
    }
}


template<typename T>
void perform_1x1(const Img_t* pInImg, const ROI_t& sInImgROI, Img_t* pOutImg, const ROI_t& sOutImgROI, 
                 FMAP* f_hori, FMAP* f_vert, FMAP* g_hori, FMAP* g_vert, FCH f_ch, 
                 const KernelCfg_t& sKerCfg){
    T* xAddr[MAX_NUM_P] = {NULL}; // stores the starting addresses of panel 0, 1, 2, ...;
    T* yAddr[MAX_NUM_P] = {NULL}; // stores the starting addresses of panel 0, 1, 2, ...;
    int i = 0; // i is row index of input image
    int j = 0; // j is column index of input image
    Formulas_1x1_T<T> Formula;
    Formula.f1x1 = (decltype(Formula.f1x1))sKerCfg.formula;
    int cList[MAX_NUM_P] = {0}; // output to which channels (panels)
    int outChNum = 0; // how many channels (panels) to output

    for (int ii = 0; ii < sOutImgROI.roiHeight; ii += sKerCfg.vertUpsample){ // ii is row index of output image
        // start addr of row i:
        yAddr[0] = (T*)(pOutImg->pImageData[0] + g_vert[0](ii)* pOutImg->strides[0]);
        yAddr[1] = (T*)(pOutImg->pImageData[1] + g_vert[1](ii)* pOutImg->strides[1]);
        yAddr[2] = (T*)(pOutImg->pImageData[2] + g_vert[2](ii)* pOutImg->strides[2]);
        xAddr[0] = (T*)(pOutImg->pImageData[0] + f_vert[0](i) * pInImg->strides[0]);
        xAddr[1] = (T*)(pOutImg->pImageData[1] + f_vert[1](i) * pInImg->strides[1]);
        xAddr[2] = (T*)(pOutImg->pImageData[2] + f_vert[2](i) * pInImg->strides[2]);
        for (int jj = 0; jj < sOutImgROI.roiWidth; jj += sKerCfg.horiUpsample){ // jj is column index of output image
            // YUV420/422 is not well-defined with step and upsample, so, if any of them is not 1 for YUV420/422, the behavior might be strange.
            yAddr[0] += g_hori[0](jj) + sKerCfg.horiUpsample; // should increment AFTER operation!!!
            yAddr[1] += g_hori[1](jj) + sKerCfg.horiUpsample;
            yAddr[2] += g_hori[2](jj) + sKerCfg.horiUpsample;
            xAddr[0] += f_hori[0](j) + sKerCfg.horiStep;
            xAddr[1] += f_hori[1](j) + sKerCfg.horiStep;
            xAddr[2] += f_hori[2](j) + sKerCfg.horiStep;

            outChNum = f_ch(cList, sOutImgROI.panelId, ii, jj);
            for(int c = 0; c < outChNum; ++c){
                *(yAddr[cList[c]]) = Formula.f1x1((const T**)xAddr, (const T**)sKerCfg.pKernel, cList[c]);
            }
            
            j += sKerCfg.horiStep;
        }
        i += sKerCfg.vertStep;
    }
}

typedef void (*F)(const Img_t*, const ROI_t&, Img_t*, const ROI_t&, 
                 FMAP*, FMAP*, FMAP*, FMAP*, FCH, 
                 const KernelCfg_t&);

IMG_RTN_CODE sliding_window_1x1(const Img_t* pInImg, const ROI_t& sInImgROI, Img_t* pOutImg, const ROI_t& sOutImgROI, const KernelCfg_t& sKerCfg){
    // change name to sliding window 1x1.
    // deal with 3p_to_3p (CCM, RGB2YUV420/422, and YUV2RGB) or 3p_to_1p (like RGB to MONO)
    // this function mainly takes care of manipulations of the third dimension and / or with step and upsample

    FMAP f_hori[3] = {NULL}; // manipulate input image iterator to grab a pixel
    FMAP f_vert[3] = {NULL}; // manipulate input image iterator to grab a pixel
    FMAP g_hori[3] = {NULL}; // manipulate output image iterator to grab a pixel
    FMAP g_vert[3] = {NULL}; // manipulate output image iterator to grab a pixel
    FCH f_ch = NULL;
    F f = NULL;

    set_in_img_iter_manipulation(pInImg->imageFormat, f_hori, f_vert);
    set_out_img_iter_manipulation(pOutImg->imageFormat, g_hori, g_vert);
    set_output_ch_manipulation(sOutImgROI.panelId, pOutImg->imageFormat, f_ch);

    if (pOutImg->sign == UNSIGNED){
        if (pOutImg->bitDepth <= 8){
            f = perform_1x1<uint8_t>;
        }
        else if (pOutImg->bitDepth <= 16){
            f = perform_1x1<uint16_t>;
        }
        else if (pOutImg->bitDepth <= 32){
            f = perform_1x1<uint32_t>;
        }
    }
    else{
        if (pOutImg->bitDepth <= 8){
            f = perform_1x1<int8_t>;
        }
        else if (pOutImg->bitDepth <= 16){
            f = perform_1x1<int16_t>;
        }
        else if (pOutImg->bitDepth <= 32){
            f = perform_1x1<int>;
        }
    }

    f(pInImg, sInImgROI, pOutImg, sOutImgROI, 
      f_hori, f_vert, g_hori, g_vert, f_ch, 
      sKerCfg);

    return SUCCEED;
}

void test_sliding_window_1x1(){
    Img_t* pImg1 =(Img_t*)malloc(sizeof(Img_t));
    IMAGE_FMT imageFormat = YUV420;
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

    // Img_t* pImg2 =(Img_t*)malloc(sizeof(Img_t));
    // construct_img(pImg2, 
    //               imageFormat,
    //               width,
    //               height,
    //               sign,
    //               bitDepth,
    //               alignment,
    //               allocateImage);

    // Formulas_1x1_T<int> Fml;
    // Fml.f1x1 = color_correction;
    // const int colorMatRow1[3] = {278, -10, -8};
    // const int colorMatRow2[3] = {-12, 269, -8};
    // const int colorMatRow3[3] = {-10, -3, 272};
    // //NOTE: should make sure that sum of column is unit one (256 or so)
    // const int* colorMatRows[3] = {colorMatRow1, colorMatRow2, colorMatRow3};
    // const KernelCfg_t sKernelCfg = {(uint8_t*)colorMatRows, // in use
    //                                 1, 1, 0, 0, // don't care for 1x1 window
    //                                 ZEROPADDING, // nonsense
    //                                 1, 1, 1, 1, // usually keep them all 1's for 1x1 window
    //                                 false,  // nonsense
    //                                 (void*)Fml.f1x1, // in use
    //                                 false // nonsense
    //                                 };

    // ROI_t sInImgROI = {1012, 0, 0, width, height}; // 1012 means panel 0, 1, 2 are all chosen
    // ROI_t sOutImgROI = {1012, 0, 0, width, height};


    // sliding_window_1x1(pImg1, sInImgROI, pImg2, sOutImgROI, sKernelCfg);

    // std::cout<<"filtered:\n";
    // ROI_t viewROI_2 = {1012,0,0,width,height};
    // view_image_data(pImg2, viewROI_2);

    // destruct_img(&pImg1);
    // destruct_img(&pImg2);
}
