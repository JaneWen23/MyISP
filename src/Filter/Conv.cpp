
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
        res += (*(*(a+i))) * (*(*(b+i)));
        res += (*(*(a+i+1))) * (*(*(b+i+1)));
        res += (*(*(a+i+2))) * (*(*(b+i+2)));
        res += (*(*(a+i+3))) * (*(*(b+i+3)));
    }
    while(i < vecLen){
        res += (*(*(a+i))) * (*(*(b+i)));
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
    T** pKerAddr = (T**)malloc(kerLen * sizeof(T*)); // stores the addr of flipped kernel
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
    uint16_t y[5] = {0};
    const KernelCfg_t sKernelCfg = {
        (uint8_t*)h, 1, 5, 2, PERIODIC, 1, 2, false};
    //=====================================

    conv_1d<uint16_t>(x, xLen, sKernelCfg, y);
    for(int i = 0; i < 5; ++i){
        std::cout<<"    "<< (int)(*(y+i)) <<", ";
    }
    std::cout<<"\n";
    
}


// the performance may case-by-case, i.e, 1d and 2d separately.


// ROI width, length and pImg->strides[c] gives the actual "x", line by line; 
// kernel shape gives vecLen to dot_product()
// padding gives the out-of-boundary pixels of x
// but how to let dot_product know that some x values are from padding scheme?



void sliding_window(Img_t* pInImg, const ROI_t& sROI, Img_t* pOutImg, const KernelCfg_t& sKernelCfg){
    // assume actual img and kernel data are int (should determine from bitDepth!!)
    // assume 1d conv
    assert(pInImg != NULL);
    assert(pOutImg != NULL);
    int* pKer =  (int*)sKernelCfg.pKernel;
    int* pInData = (int*)(pInImg->pImageData);
    int* pOutData = (int*)(pOutImg->pImageData);
    int strideInPix = pInImg->strides[sROI.panelId] / sizeof(int);

    
    for (int i = 0; i < sROI.roiHeight; ++i){ 
        for(int j = 0; j < sROI.roiWidth; ++j){ 
            // out img (i,j) to in img row and col:
            // ori_i = rowStep*i + sROI.startRow; ori_j = j + sROI.startCol 
            // (i*roiWidth+j) --> (ori_i*strideInPix + colStep*j)
            // need to find, or define the center of the kernel, and flip the kernel
            //*(pOutData + i*strideInPix + j) = ...???
        }
    }

}



void test_conv(){
    // int a[11] = {2,1,1,1,1,1,1,1,1,1,1};
    // int b[11] = {1,1,1,1,1,1,1,1,1,1,1};

    // int res = dot_product<int>(a, b, 1);
    // std::cout<<"dot product result = "<< res<<"\n";
    //std::cout<<"bool="<<(0 < -2)<<"\n";

    test_dot_product_clever();
    //test_set_addr_array_2();
}
