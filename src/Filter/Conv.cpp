
#include "Conv.hpp"

// sliding window: a uniform API to perform 1D/2D convolution of any kernel, with step configurable.


// TODO: openMP??? -O2? -O3?
template<typename T>
const T dot_product(const T* a, const T* b, const int vecLen){
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
T conv_1d(const T* x, const int xLen, const T* h, const int hLen){
    // don't forget to flip the kernel!!
    //return dot_product<T>(x, h, hLen); // to be removed
    
}



// the performance may case-by-case, i.e, 1d and 2d separately.


// ROI width, length and pImg->strides[c] gives the actual "x", line by line; 
// kernel shape gives vecLen to dot_product()
// padding gives the out-of-boundary pixels of x
// but how to let dot_product know that some x values are from padding scheme?
// padding is actualy map the out-of-boundary index to a valid index. so dot_product may not be used


// TODO: find_kernel_center(); flip_kernel();

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
            *(pOutData + i*strideInPix + j) = ...???
        }
    }

}



void test_conv(){
    int a[11] = {2,1,1,1,1,1,1,1,1,1,1};
    int b[11] = {1,1,1,1,1,1,1,1,1,1,1};

    int res = dot_product<int>(a, b, 1);
    std::cout<<"dot product result = "<< res<<"\n";
    //std::cout<<"bool="<<(0 < -2)<<"\n";
}
