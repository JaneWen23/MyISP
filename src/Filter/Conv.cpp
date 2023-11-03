
#include "Conv.hpp"

// sliding window: a uniform API to perform 1D/2D convolution of any kernel, with step configurable.


// TODO: openMP??? -O2? -O3?
template<typename T>
const T dot_product(const T* a, const T* b, const int vecLen){ // not in use
    T res = 0;
    int i = 0;
    // parallel computation, maybe
    for (i = 0; i < vecLen - 4; i += 4){
        res += (*(a+i)) * (*(b+i));
        res += (*(a+i+1)) * (*(b+i+1));
        res += (*(a+i+2)) * (*(b+i+2));
        res += (*(a+i+3)) * (*(b+i+3));
    }
    while(i < vecLen){
        res += (*(a+i)) * (*(b+i));
        ++i;
    }
    return res;
}

template<typename T>
const T dot_product_clever(const T** a, const T** b, const int vecLen){ // in use
    // "T** a" is to help handel the padded signal.
    // "T** b" is to help handel the flipped kernel.
    T res = 0;
    int i = 0;
    // parallel computation, maybe
    for(i = 0; i < vecLen - 4; i += 4){
        res += (**(a+i)) * (**(b+i));
        res += (**(a+i+1)) * (**(b+i+1));
        res += (**(a+i+2)) * (**(b+i+2));
        res += (**(a+i+3)) * (**(b+i+3));
    }
    while(i < vecLen){
        res += (**(a+i)) * (**(b+i));
        ++i;
    }
    return res;
}

template<typename T>
void set_kernel_addr(T** pAddr, T* pKernel, int kerLen, bool needFlip){
    if (needFlip){
        for(int i = kerLen - 1; i >= 0; --i){
            pAddr[kerLen - 1 - i] = pKernel + i;
        }
    }
    else{
        for(int i = 0; i < kerLen; ++i){
            pAddr[i] = pKernel + i;
        }
    }
    
}

template<typename T>
void set_addr_array(T** pAddr, const T* px, const int xLen, const int lenPadded, const int kerCenter, const PADDING& padding, const T* pz = NULL){
    // "pz" is not needed if padding is not ZEROPADDING.
    // TODO: if padding is mirror, must ensure that hLen <= xLen!!! because mirror padding can only extend 1x.
    switch (padding){
        case ZEROPADDING:{
            assert(pz != NULL);
            for (int i = 0; i < kerCenter; ++i){
                pAddr[i] = (T*)pz; // the addr of zero.
                // "(T*)" converts "const T*" to "T*", to make compiler happy.
            }
            for (int i = kerCenter; i < kerCenter + xLen; ++i){
                pAddr[i] = (T*)px + i - kerCenter;
            }
            for (int i = kerCenter + xLen; i < lenPadded; ++i){
                pAddr[i] = (T*)pz; // the addr of zero.
            }
            break;
        }
        case PERIODIC:{
            for (int i = 0; i < lenPadded; ++i){
                pAddr[i] = (T*)px + ((i - kerCenter) % xLen + xLen) % xLen; // "+xLen" and "%xLen again" is to handle the negative indicies
            }
            break;
        }
        case MIRROR:{
            for (int i = 0; i < kerCenter; ++i){
                pAddr[i] = (T*)px + kerCenter-i;
            }
            for (int i = kerCenter; i < kerCenter + xLen; ++i){
                pAddr[i] = (T*)px + i - kerCenter;
            }
            for (int i = kerCenter + xLen; i < lenPadded; ++i){
                pAddr[i] = (T*)px + 2 * (xLen - 1) - (i - kerCenter);
            }
            break;
        }
        default:{
            break;
        }
    }
}



template<typename T>
void conv_1d(const T* x, const int xLen, const KernelCfg_t& sKernelCfg, T* y){
    int kerLen = sKernelCfg.nCols; // TODO: all things in conv_1d are horizontal 1d. need to support vertical 1d!!!
    int lenPadded = xLen + kerLen - 1;
    T** pAddr = (T**)malloc(lenPadded * sizeof(T*)); // stores the addr of padded x
    const T z = 0; // this is useful for zero-padding

    set_addr_array<T>(pAddr, x, xLen, lenPadded, sKernelCfg.centerX, sKernelCfg.padding, &z);
    // for(int i = 0; i < lenPadded; ++i){
    //     std::cout<<"    "<< (int)(*(pAddr[i]))<<", ";
    // }
    // std::cout<<"\n";
    T** pKerAddr = (T**)malloc(kerLen * sizeof(T*)); // stores the addr of flipped (or not) kernel
    set_kernel_addr<T>(pKerAddr, (T*)sKernelCfg.pKernel, kerLen, sKernelCfg.needFlip);
    // for(int i = 0; i < kerLen; ++i){
    //     std::cout<<"    "<< (int)(*(pKerAddr[i]))<<", ";
    // }
    // std::cout<<"\n";

    for(int i = 0; i < xLen; i += sKernelCfg.colStep){
        *(y + (i/sKernelCfg.colStep)) = dot_product_clever<T>((const T **)pAddr + i, (const T **)pKerAddr, kerLen);
    }
    
}


void test_dot_product_clever(){
    uint16_t x[10] = {1, 2, 3, 1, 1, 1, 7, 6, 5, 3};
    int xLen = 10;
    uint16_t h[5] = {1, 1, 1, 0, 0};
    uint16_t y[10] = {0}; // length of y should be xLen / kernelStep
    const KernelCfg_t sKernelCfg = {
        (uint8_t*)h, 1, 5, 2, PERIODIC, 1, 1, false};
    //=====================================

    conv_1d<uint16_t>(x, xLen, sKernelCfg, y);
    for(int i = 0; i < 10; ++i){
        std::cout<<"    "<< (int)(*(y+i)) <<", ";
    }
    std::cout<<"\n";
    
}


// the performance may case-by-case, i.e, 1d and 2d separately.
template<typename T>
void perform_conv_1d_horizontal(Img_t* pInImg, const ROI_t& sInImgROI, Img_t* pOutImg, const ROI_t& sOutImgROI, const KernelCfg_t& sKernelCfg){
    // perform 1d convolution in every row
    int inImgStride = pInImg->strides[sInImgROI.panelId];
    int outImgStride = pOutImg->strides[sOutImgROI.panelId];
    uint8_t* x = pInImg->pImageData[sInImgROI.panelId] + sInImgROI.startRow * inImgStride + sInImgROI.startCol * sizeof(T);
    uint8_t* y = pOutImg->pImageData[sOutImgROI.panelId]+ sOutImgROI.startRow * outImgStride + sOutImgROI.startCol * sizeof(T);
    for (int i = 0; i < sInImgROI.roiHeight; ++i){
        conv_1d<T>((T*)x, sInImgROI.roiWidth, sKernelCfg, (T*)y);
        x += inImgStride;
        y += outImgStride;
    }
}

void sliding_window(Img_t* pInImg, const ROI_t& sInImgROI, Img_t* pOutImg, const ROI_t& sOutImgROI, const KernelCfg_t& sKernelCfg){
    assert(pInImg != NULL);
    assert(pOutImg != NULL);
    
    // horizontal 1d convolution
    if (pOutImg->sign == UNSIGNED){
        if (pOutImg->bitDepth <= 8){
            perform_conv_1d_horizontal<uint8_t>(pInImg, sInImgROI, pOutImg, sOutImgROI, sKernelCfg);
        }
        else if (pOutImg->bitDepth <= 16){
            perform_conv_1d_horizontal<uint16_t>(pInImg, sInImgROI, pOutImg, sOutImgROI, sKernelCfg);
        }
        else if (pOutImg->bitDepth <= 32){
            perform_conv_1d_horizontal<uint32_t>(pInImg, sInImgROI, pOutImg, sOutImgROI, sKernelCfg);
        }
    }
    else{
        if (pOutImg->bitDepth <= 8){
            perform_conv_1d_horizontal<int8_t>(pInImg, sInImgROI, pOutImg, sOutImgROI, sKernelCfg);
        }
        else if (pOutImg->bitDepth <= 16){
            perform_conv_1d_horizontal<int16_t>(pInImg, sInImgROI, pOutImg, sOutImgROI, sKernelCfg);
        }
        else if (pOutImg->bitDepth <= 32){
            perform_conv_1d_horizontal<int>(pInImg, sInImgROI, pOutImg, sOutImgROI, sKernelCfg);
        }
    }
    
}



void test_conv(){
    //test_dot_product_clever();

    Img_t* pImg1 =(Img_t*)malloc(sizeof(Img_t));
    IMAGE_FMT imageFormat = RGB; // out can be different than in
    size_t width = 20; // out can be different than in
    size_t height = 12; // out can be different than in
    SIGN sign = SIGNED; // out can be different than in
    size_t bitDepth = 16; // out and kernel must be the same as in, and you should be careful about the sign,
    // i.e., if out img is signed but in img is unsigned, since the in img data type will be treated as out img data type,
    // the large unsigned values (from "in img") will be interpreted into negative signed values; to prevent this, you
    // may choose a "larger" data type for in img.
    size_t alignment = 32; // out must be the same as in
    bool allocateImage = true; // recommend that out = in = true

    construct_img(pImg1, 
                  imageFormat,
                  width,
                  height,
                  sign,
                  bitDepth,
                  alignment,
                  allocateImage);
    
    ValCfg_t sValCfg = {pImg1->sign, rand_num_uniform, {0, 99, 0, 0}};

    set_value(pImg1, sValCfg);
    view_img_properties(pImg1);

    std::cout<<"original:\n";
    ROI_t viewROI_1 = {0,0,0,width,height};
    view_image_data(pImg1, viewROI_1);

    Img_t* pImg2 =(Img_t*)malloc(sizeof(Img_t));
    construct_img(pImg2, 
                  imageFormat,
                  25,
                  height,
                  sign,
                  bitDepth,
                  alignment,
                  allocateImage);

    uint16_t h[5] = {0, 0, 1, 0, 0}; // should be matched with Img_t bitDepth!!
    const KernelCfg_t sKernelCfg = {
        (uint8_t*)h, 1, 5, 2, PERIODIC, 1, 1, false};

    ROI_t sInImgROI = {0, 3, 2, 15, 9};
    ROI_t sOutImgROI = {1, 1, 1, 15, 9}; // may create a helper function; 
    // if format is mono, panel id must be 0; should also check img width height vs ROI width height and kernel step
    // or create a helper function for out img width height

    sliding_window(pImg1, sInImgROI, pImg2, sOutImgROI, sKernelCfg);

    std::cout<<"filtered:\n";
    ROI_t viewROI_2 = {1,0,0,8,10};
    view_image_data(pImg2, viewROI_2);

    destruct_img(&pImg1);
    destruct_img(&pImg2);
}
