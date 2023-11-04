
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
void set_addr_array(T** pAddr, const T* x, const int xLen, const int lenPadded, const int horiCenter, const PADDING& padding, const T* pz = NULL){
    // "pz" is not needed if padding is not ZEROPADDING.
    // TODO: if padding is mirror, must ensure that hLen <= xLen!!! because mirror padding can only extend 1x.
    switch (padding){
        case ZEROPADDING:{
            assert(pz != NULL);
            for (int i = 0; i < horiCenter; ++i){
                pAddr[i] = (T*)pz; // the addr of zero.
                // "(T*)" converts "const T*" to "T*", to make compiler happy.
            }
            for (int i = horiCenter; i < horiCenter + xLen; ++i){
                pAddr[i] = (T*)x + i - horiCenter;
            }
            for (int i = horiCenter + xLen; i < lenPadded; ++i){
                pAddr[i] = (T*)pz; // the addr of zero.
            }
            break;
        }
        case PERIODIC:{
            for (int i = 0; i < lenPadded; ++i){
                pAddr[i] = (T*)x + ((i - horiCenter) % xLen + xLen) % xLen; // "+xLen" and "%xLen again" is to handle the negative indicies
            }
            break;
        }
        case MIRROR:{
            for (int i = 0; i < horiCenter; ++i){
                pAddr[i] = (T*)x + horiCenter-i;
            }
            for (int i = horiCenter; i < horiCenter + xLen; ++i){
                pAddr[i] = (T*)x + i - horiCenter;
            }
            for (int i = horiCenter + xLen; i < lenPadded; ++i){
                pAddr[i] = (T*)x + 2 * (xLen - 1) - (i - horiCenter);
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
    int kerLen = sKernelCfg.kerWidth;
    int lenPadded = xLen + kerLen - 1;
    T** pAddr = (T**)malloc(lenPadded * sizeof(T*)); // stores the addr of padded x
    const T z = 0; // this is useful for zero-padding

    set_addr_array<T>(pAddr, x, xLen, lenPadded, sKernelCfg.horiCenter, sKernelCfg.padding, &z);
    // for(int i = 0; i < lenPadded; ++i){
    //     std::cout<<"    "<< (int)(*(pAddr[i]))<<", ";
    // }
    // std::cout<<"\n";
    T** pKerAddr = (T**)malloc(kerLen * sizeof(T*)); // stores the addr of flipped (or not) kernel; TODO: may move out of this func
    set_kernel_addr<T>(pKerAddr, (T*)sKernelCfg.pKernel, kerLen, sKernelCfg.needFlip);
    // for(int i = 0; i < kerLen; ++i){
    //     std::cout<<"    "<< (int)(*(pKerAddr[i]))<<", ";
    // }
    // std::cout<<"\n";

    for(int i = 0; i < xLen; i += sKernelCfg.horiStep){
        *(y + (i/sKernelCfg.horiStep)) = dot_product_clever<T>((const T **)pAddr + i, (const T **)pKerAddr, kerLen);
        // if vertical or 2D: break into sum of #_of_lines' dot products, treat the row direction as automatically operated and need not taking care.
    }
    
}


const uint8_t* vert_padding_map(const int ind, const uint8_t* x, const int roiHeight, const int inImgStride, const PADDING& padding, const uint8_t* zVec){
    switch (padding){
        case ZEROPADDING:{
            assert(zVec != NULL);
            if(ind < 0){
                return zVec;
            }
            else if (ind >= roiHeight){
                return zVec;
            }
            else{
                return x + ind * inImgStride;
            }
            break;
        }
        case PERIODIC:{
            return x + ((roiHeight + ind) % roiHeight) * inImgStride;
            break;
        }
        case MIRROR:{
            if(ind < 0){
                return x - ind * inImgStride;
            }
            else if (ind >= roiHeight){
                return x + (2 * (roiHeight - 1) - ind) * inImgStride;
            }
            else{
                return x + ind * inImgStride;
            }
            break;
        }
        default:{
            return NULL;
            break;
        }
    }
}


template<typename T>
void conv_2d_meta(const T** px, const int xWidth, 
                  const T** pKerAddrMatrix, 
                  const int kerWidth, const int kerHeight, 
                  const int horiCenter, const int horiStep, 
                  const PADDING& padding, 
                  T* y){
    // px[i] stored the head address of i-th line of x, i is in terms of kernel height
    // outside this func: need augumented "x", i.e., pad the non-existed rows;
    // inside this func: pad the x horizontally, stack this operation for all #_kernel_height lines
   
    // kerWidth: to help with horizontal padding len
    // kerHeight: to determine how many rows needed for the 2d convolution to output one line
    int xWidthPadded = xWidth + kerWidth - 1;
    T yTemp = 0;
    T** pAddrMatrix = (T**)malloc(kerHeight * xWidthPadded * sizeof(T*));
    const T z = 0;
    
    for (int i = 0; i < kerHeight; ++i){
        set_addr_array<T>(pAddrMatrix+i*xWidthPadded, px[i], xWidth, xWidthPadded, horiCenter, padding, &z);
    }

    for(int j = 0; j < xWidth; j += horiStep){
        yTemp = 0;
        for(int i = 0; i < kerHeight; ++i){
            yTemp += dot_product_clever<T>((const T **)pAddrMatrix + j + i*xWidthPadded, (const T **)pKerAddrMatrix+i*kerWidth, kerWidth);
        }
        *(y + (j/horiStep)) = yTemp;
    }

}

template<typename T>
void conv_1d_vertical_meta(const T** px, const int xRoiWidth, const T ** pKerAddr, const int kerHeight, const int horiStep, T* y){
    for(int j = 0; j < xRoiWidth; j += horiStep){
        *(y + (j/horiStep)) = dot_product_clever<T>((const T **)px, (const T **)pKerAddr, kerHeight);
        for(int i = 0; i < kerHeight; ++i){
            px[i] += 1;
        }
    }
}

template<typename T>
void test_conv_2d_meta(const Img_t* pInImg, const ROI_t& sInImgROI, Img_t* pOutImg, const ROI_t& sOutImgROI, const KernelCfg_t& sKernelCfg){
    int inImgStride = pInImg->strides[sInImgROI.panelId];
    int outImgStride = pOutImg->strides[sOutImgROI.panelId];
    uint8_t* x = pInImg->pImageData[sInImgROI.panelId] + sInImgROI.startRow * inImgStride + sInImgROI.startCol * sizeof(T);
    uint8_t* y = pOutImg->pImageData[sOutImgROI.panelId]+ sOutImgROI.startRow * outImgStride + sOutImgROI.startCol * sizeof(T);

    uint8_t** px = (uint8_t**)malloc(sKernelCfg.kerHeight * sizeof(T*));
    uint8_t* zVec = (uint8_t*)malloc(sInImgROI.roiWidth * sizeof(T));
    memset(zVec, 0, sizeof(T));

    T** pKerAddrMatrix = (T**)malloc(sKernelCfg.kerHeight * sKernelCfg.kerWidth * sizeof(T*)); // stores the addr of horizontally flipped (or not) kernel;
    for (int i = 0; i < sKernelCfg.kerHeight; ++i){
        set_kernel_addr<T>(pKerAddrMatrix + i*sKernelCfg.kerWidth, (T*)sKernelCfg.pKernel + i*sKernelCfg.kerWidth, sKernelCfg.kerWidth, sKernelCfg.needFlip);
    }
    // for(int i = 0; i < sKernelCfg.kerWidth * sKernelCfg.kerHeight; ++i){
    //     std::cout<<"    "<< (int)(*(pKerAddrMatrix[i]))<<", ";
    // }
    // std::cout<<"\n";
    // for(int i = 0; i < sKernelCfg.kerWidth * sKernelCfg.kerHeight; ++i){
    //     std::cout<<"    "<< pKerAddrMatrix[i]<<", ";
    // }
    // std::cout<<"\n";

    // TODO: then, need vertically flipped kernel!!!

    for (int i = 0; i < sInImgROI.roiHeight; i += sKernelCfg.vertStep){

        for (int l = -sKernelCfg.vertCenter; l < sKernelCfg.kerHeight - sKernelCfg.vertCenter; ++l){
            px[l + sKernelCfg.vertCenter] = (uint8_t*)vert_padding_map(i + l, (const uint8_t*)x, sInImgROI.roiHeight, inImgStride, sKernelCfg.padding, (const uint8_t*)zVec);
            //std::cout<<"*px[k]= "<<*((T*)px[k])<<"\n";
        }
        
        // conv_2d_meta<T>((const T**)px, sInImgROI.roiWidth, 
        //                 (const T**)pKerAddrMatrix, 
        //                 sKernelCfg.kerWidth, sKernelCfg.kerHeight, 
        //                 sKernelCfg.horiCenter, sKernelCfg.horiStep, 
        //                 sKernelCfg.padding,
        //                 (T*)y);

        conv_1d_vertical_meta<T>((const T**)px, sInImgROI.roiWidth, (const T**)pKerAddrMatrix, sKernelCfg.kerHeight, sKernelCfg.horiStep, (T*)y);

        y += outImgStride;
    }
}


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
    size_t bitDepth = 32; // out and kernel must be the same as in, and you should be careful about the sign,
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
    
    ValCfg_t sValCfg = {pImg1->sign, rand_num_uniform, {0, 49, 0, 0}};

    set_value(pImg1, sValCfg);
    view_img_properties(pImg1);

    std::cout<<"original:\n";
    ROI_t viewROI_1 = {0,0,0,width,height};
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

    // uint32_t h[6] = {0, 1,
    //                  1, 1,
    //                  0, 0}; // should be matched with Img_t bitDepth!!
    uint32_t h[5] = {0, 1, 1, 0, 0}; // should be matched with Img_t bitDepth!!
    //const KernelCfg_t sKernelCfg = {
        // (uint8_t*)h, 3, 2, 0, 1, ZEROPADDING, 1, 1, false};
    const KernelCfg_t sKernelCfg = {
    (uint8_t*)h, 5, 1, 0, 2, ZEROPADDING, 1, 1, false};

    ROI_t sInImgROI = {0, 0, 0, width, height};
    ROI_t sOutImgROI = {0, 0, 0, width, height}; // TODO: may create a helper function to find ROI (???) based on kernel; 
    // TODO: create a helper function for out img width height and kernel step

    //sliding_window(pImg1, sInImgROI, pImg2, sOutImgROI, sKernelCfg);
    test_conv_2d_meta<int>(pImg1, sInImgROI, pImg2, sOutImgROI, sKernelCfg);

    std::cout<<"filtered:\n";
    ROI_t viewROI_2 = {0,0,0,width,height};
    view_image_data(pImg2, viewROI_2);

    destruct_img(&pImg1);
    destruct_img(&pImg2);
}
