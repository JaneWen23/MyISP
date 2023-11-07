
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
const T my_formula(const T** a, const T** b, const int vecLen){
    T res = 0;
    for(int i = 0; i < vecLen; ++i){
        res += (**(a+i) > **(b+i))? **(a+i) : **(b+i);
    }
    return res;
}

// these func pointers can point to either my_formula or dot_product_clever
typedef const uint8_t (*FP_MFu8)(const uint8_t**, const uint8_t**, const int);
typedef const uint16_t (*FP_MFu16)(const uint16_t**, const uint16_t**, const int);
typedef const uint32_t (*FP_MFu32)(const uint32_t**, const uint32_t**, const int);
typedef const int8_t (*FP_MFs8)(const int8_t**, const int8_t**, const int);
typedef const int16_t (*FP_MFs16)(const int16_t**, const int16_t**, const int);
typedef const int (*FP_MFs32)(const int**, const int**, const int);

template<typename T>
struct MyStruct{
    const T (*FP_MF)(const T**, const T**, const int);
}; // note: cannot typedef, because there is no type yet.

// void test_my_struct(){
//     //pRealStruct->FP_MFu8 = my_formula; // no need to type my_formula<int> anymore, because it follows the struct type!!
//     //pRealStruct->FP_MFu16 = dot_product_clever; // no need to type my_formula<int> anymore, because it follows the struct type!!
// }

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
void set_addr_array(T** pAddr, const T* x, const int xWidth, const int lenPadded, const int horiCenter, const PADDING& padding, const T* pz = NULL){
    // "pz" is not needed if padding is not ZEROPADDING.
    // TODO: if padding is mirror, must ensure that hLen <= xWidth!!! because mirror padding can only extend 1x.
    switch (padding){
        case ZEROPADDING:{
            assert(pz != NULL);
            for (int i = 0; i < horiCenter; ++i){
                pAddr[i] = (T*)pz; // the addr of zero.
                // "(T*)" converts "const T*" to "T*", to make compiler happy.
            }
            for (int i = horiCenter; i < horiCenter + xWidth; ++i){
                pAddr[i] = (T*)x + i - horiCenter;
            }
            for (int i = horiCenter + xWidth; i < lenPadded; ++i){
                pAddr[i] = (T*)pz; // the addr of zero.
            }
            break;
        }
        case PERIODIC:{
            for (int i = 0; i < lenPadded; ++i){
                pAddr[i] = (T*)x + ((i - horiCenter) % xWidth + xWidth) % xWidth; // "+xWidth" and "%xWidth again" is to handle the negative indicies
            }
            break;
        }
        case MIRROR:{
            for (int i = 0; i < horiCenter; ++i){
                pAddr[i] = (T*)x + horiCenter-i;
            }
            for (int i = horiCenter; i < horiCenter + xWidth; ++i){
                pAddr[i] = (T*)x + i - horiCenter;
            }
            for (int i = horiCenter + xWidth; i < lenPadded; ++i){
                pAddr[i] = (T*)x + 2 * (xWidth - 1) - (i - horiCenter);
            }
            break;
        }
        default:{
            break;
        }
    }
}

template<typename T>
void addr_matrix_transpose(T** pSrc, const int srcHeight, const int srcWidth, T** pDst){
    for (int i = 0; i < srcHeight; ++i){
        for (int j = 0; j < srcWidth; ++j){
            *(pDst + j*srcHeight + i) = *(pSrc + i*srcWidth + j);
        }
    }
    // or, for i = 0, 1, ..., srcHeight-1:
    // mapped(i) = (i % srcHeight) * srcWidth + i
    // mapped(i) is the index of pDst to be set value of pSrc[i].
    // TODO: may set up a timer to see which way is better.
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
void conv_1d_deprecated(const T* x, const int xLen, const KernelCfg_t& sKernelCfg, T* y){
    int kerLen = sKernelCfg.kerWidth;
    int lenPadded = xLen + kerLen - 1;
    T** pAddr = (T**)malloc(lenPadded * sizeof(T*)); // stores the addr of padded x
    const T z = 0; // this is useful for zero-padding

    set_addr_array<T>(pAddr, x, xLen, lenPadded, sKernelCfg.horiCenter, sKernelCfg.padding, &z);
    // for(int i = 0; i < lenPadded; ++i){
    //     std::cout<<"    "<< (int)(*(pAddr[i]))<<", ";
    // }
    // std::cout<<"\n";
    T** pKerAddr = (T**)malloc(kerLen * sizeof(T*)); // stores the addr of flipped (or not) kernel;
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

template<typename T>
void conv_2d_unit(const T** px, const int xWidth, 
                  const T** pKerAddrMatrix, 
                  const KernelCfg_t& sKernelCfg, 
                  T* y){
    // px[i] stored the head address of i-th line of x, i is in terms of kernel height
    // outside this func: need augumented "x", i.e., pad the non-existed rows;
    // inside this func: pad the x horizontally, stack this operation for all #_kernel_height lines
   
    // kerWidth: to help with horizontal padding len
    // kerHeight: to determine how many rows needed for the 2d convolution to output one line
    int xWidthPadded = xWidth + sKernelCfg.kerWidth - 1;
    T yTmp = 0;
    T** pAddrMatrix = (T**)malloc(sKernelCfg.kerHeight * xWidthPadded * sizeof(T*));
    const T z = 0;
    MyStruct<T> myRealStruct;
    myRealStruct.FP_MF = (decltype(myRealStruct.FP_MF))(sKernelCfg.formula);
    
    for (int i = 0; i < sKernelCfg.kerHeight; ++i){
        set_addr_array<T>(pAddrMatrix+i*xWidthPadded, px[i], xWidth, xWidthPadded, sKernelCfg.horiCenter, sKernelCfg.padding, &z);
    }

    for(int j = 0; j < xWidth; j += sKernelCfg.horiStep){
        // this is to some extent seperable: "2d dot product" is the sum of several 1d dot products
        // if a 2d kernel cannot be broken into manipulation of several 1d, 
        // you should first transpose the "pAddrMatrix" outside this loop,
        // then apply the flatten kernel here (increment of transposed (and flatten) pAddrMatrix is horiStep * kerHeight).
        yTmp = 0;
        for(int i = 0; i < sKernelCfg.kerHeight; ++i){
            // yTmp += dot_product_clever<T>((const T **)pAddrMatrix + j + i*xWidthPadded, (const T **)pKerAddrMatrix+i*sKernelCfg.kerWidth, sKernelCfg.kerWidth);
            yTmp += myRealStruct.FP_MF((const T **)pAddrMatrix + j + i*xWidthPadded, (const T **)pKerAddrMatrix+i*sKernelCfg.kerWidth, sKernelCfg.kerWidth);
        }
        *(y + (j/sKernelCfg.horiStep)) = yTmp;
    }

}

template<typename T>
void conv_2d(const uint8_t* x, const int xWidth, const int xHeight,
             const T** pKerAddrMatrix, const KernelCfg_t& sKernelCfg, 
             const int inImgStride, const int outImgStride, 
             uint8_t* y){
    uint8_t** px = (uint8_t**)malloc(sKernelCfg.kerHeight * sizeof(T*));
    uint8_t* zVec = (uint8_t*)malloc(xWidth * sizeof(T));
    memset(zVec, 0, xWidth * sizeof(T));
    for (int i = 0; i < xHeight; i += sKernelCfg.vertStep){
        for (int l = -sKernelCfg.vertCenter; l < sKernelCfg.kerHeight - sKernelCfg.vertCenter; ++l){
            px[l + sKernelCfg.vertCenter] = (uint8_t*)vert_padding_map(i + l, x, xHeight, inImgStride, sKernelCfg.padding, (const uint8_t*)zVec);
        }
        
        conv_2d_unit<T>((const T**)px, xWidth, 
                        pKerAddrMatrix, 
                        sKernelCfg,
                        (T*)y);

        y += outImgStride;
    }
}


template<typename T>
void conv_1d_vertical_unit(const T** px, const int xRoiWidth, 
                           const T** pKerAddr, 
                           const KernelCfg_t& sKernelCfg, 
                           T* y){
    for(int j = 0; j < xRoiWidth; j += sKernelCfg.horiStep){
        *(y + (j/sKernelCfg.horiStep)) = dot_product_clever<T>((const T **)px, (const T **)pKerAddr, sKernelCfg.kerHeight);
        for(int i = 0; i < sKernelCfg.kerHeight; ++i){
            px[i] += 1;
        }
    }
}

template<typename T>
void conv_1d_vertical(const uint8_t* x, const int xWidth, const int xHeight,
             const T** pKerAddrMatrix, const KernelCfg_t& sKernelCfg, 
             const int inImgStride, const int outImgStride, 
             uint8_t* y){
    uint8_t** px = (uint8_t**)malloc(sKernelCfg.kerHeight * sizeof(T*));
    uint8_t* zVec = (uint8_t*)malloc(xWidth * sizeof(T));
    memset(zVec, 0, xWidth * sizeof(T));
    for (int i = 0; i < xHeight; i += sKernelCfg.vertStep){
        for (int l = -sKernelCfg.vertCenter; l < sKernelCfg.kerHeight - sKernelCfg.vertCenter; ++l){
            px[l + sKernelCfg.vertCenter] = (uint8_t*)vert_padding_map(i + l, x, xHeight, inImgStride, sKernelCfg.padding, (const uint8_t*)zVec);
        }
        
        conv_1d_vertical_unit<T>((const T**)px, xWidth, 
                        pKerAddrMatrix, 
                        sKernelCfg,
                        (T*)y);

        y += outImgStride;
    }
}

template<typename T>
void conv_1d_horizontal_unit(const T** px, const int xWidth, 
                             const T** pKerAddr, 
                             const KernelCfg_t& sKernelCfg,  
                             T* y){
    int xWidthPadded = xWidth + sKernelCfg.kerWidth - 1;
    T** pAddrArray = (T**)malloc(xWidthPadded * sizeof(T*)); // stores the addr of padded x
    const T z = 0; // this is useful for zero-padding

    set_addr_array<T>(pAddrArray, px[0], xWidth, xWidthPadded, sKernelCfg.horiCenter, sKernelCfg.padding, &z);

    for(int i = 0; i < xWidth; i += sKernelCfg.horiStep){
        *(y + (i/sKernelCfg.horiStep)) = dot_product_clever<T>((const T **)pAddrArray + i, (const T **)pKerAddr, sKernelCfg.kerWidth);
    }
}

template<typename T>
void conv_1d_horizontal(const uint8_t* x, const int xWidth, const int xHeight,
             const T** pKerAddrMatrix, const KernelCfg_t& sKernelCfg, 
             const int inImgStride, const int outImgStride, 
             uint8_t* y){
    for (int i = 0; i < xHeight; i += sKernelCfg.vertStep){
        conv_1d_horizontal_unit<T>((const T**)(&x), xWidth, 
                        pKerAddrMatrix, 
                        sKernelCfg,
                        (T*)y);
        x += sKernelCfg.vertStep * inImgStride;
        y += outImgStride;
    }
}

template<typename T>
void conv(const uint8_t* x, const int inImgStride, const int inImgRoiWidth, const int inImgRoiHeight,
                  uint8_t* y, const int outImgStride,
                  const KernelCfg_t& sKernelCfg){
    T** pKerAddrMatrix = (T**)malloc(sKernelCfg.kerHeight * sKernelCfg.kerWidth * sizeof(T*)); // stores the addr of flipped (or not) kernel;
    set_kernel_addr<T>(pKerAddrMatrix, (T*)sKernelCfg.pKernel, sKernelCfg.kerHeight * sKernelCfg.kerWidth, sKernelCfg.needFlip);
    
    if (sKernelCfg.kerHeight == 1){
        conv_1d_horizontal<T>(x, inImgRoiWidth, inImgRoiHeight,
            (const T**)pKerAddrMatrix, sKernelCfg, 
            inImgStride, outImgStride, 
            y);
    }
    else if (sKernelCfg.kerWidth == 1){
        conv_1d_vertical<T>(x, inImgRoiWidth, inImgRoiHeight,
            (const T**)pKerAddrMatrix, sKernelCfg, 
            inImgStride, outImgStride, 
            y);
    }
    else{
        conv_2d<T>(x, inImgRoiWidth, inImgRoiHeight,
            (const T**)pKerAddrMatrix, sKernelCfg, 
            inImgStride, outImgStride, 
            y);
    }
}

typedef void (*FP_CONV)(const uint8_t*, const int, const int, const int,
                  uint8_t*, const int, const KernelCfg_t&);

void sliding_window(Img_t* pInImg, const ROI_t& sInImgROI, Img_t* pOutImg, const ROI_t& sOutImgROI, const KernelCfg_t& sKernelCfg){
    assert(pInImg != NULL);
    assert(pOutImg != NULL);
    int inImgStride = pInImg->strides[sInImgROI.panelId];
    int outImgStride = pOutImg->strides[sOutImgROI.panelId];
    int scale = 0;
    FP_CONV f = NULL;

    if (pOutImg->sign == UNSIGNED){
        if (pOutImg->bitDepth <= 8){
            scale = sizeof(uint8_t);
            f = conv<uint8_t>;
        }
        else if (pOutImg->bitDepth <= 16){
            scale = sizeof(uint16_t);
            f = conv<uint16_t>;
        }
        else if (pOutImg->bitDepth <= 32){
            scale = sizeof(uint32_t);
            f = conv<uint32_t>;
        }
    }
    else{
        if (pOutImg->bitDepth <= 8){
            scale = sizeof(int8_t);
            f = conv<int8_t>;
        }
        else if (pOutImg->bitDepth <= 16){
            scale = sizeof(int16_t);
            f = conv<int16_t>;
        }
        else if (pOutImg->bitDepth <= 32){
            scale = sizeof(int);
            f = conv<int>;
        }
    }

    const uint8_t* x = pInImg->pImageData[sInImgROI.panelId] + sInImgROI.startRow * inImgStride + sInImgROI.startCol * scale;
    uint8_t* y = pOutImg->pImageData[sOutImgROI.panelId]+ sOutImgROI.startRow * outImgStride + sOutImgROI.startCol * scale;

    
    f(x, inImgStride, sInImgROI.roiWidth, sInImgROI.roiHeight,
                  y, outImgStride,
                  sKernelCfg);
    
}

template<typename T>
void test_my_ptr(void* myPtr){
    MyStruct<T> myRealStruct;
    myRealStruct.FP_MF = (decltype(myRealStruct.FP_MF))myPtr;
    T res = 0;
    T x = 3;
    T y = 1;
    const T* a = &x;
    const T* b = &y;
    const T** aa = &a;
    const T** bb = &b;
    res = myRealStruct.FP_MF(aa, bb, 1); // note: a function can be called like this!!
    std::cout<<"res = "<<res<<"\n";
}


void test_conv(){
    //test_dot_product_clever();

    Img_t* pImg1 =(Img_t*)malloc(sizeof(Img_t));
    IMAGE_FMT imageFormat = RGB; // out can be different than in
    size_t width = 10; // out can be different than in
    size_t height = 8; // out can be different than in
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

    MyStruct<int>* pRealStruct = (MyStruct<int>*)malloc(sizeof(MyStruct<int>)); // test my struct
    pRealStruct->FP_MF = my_formula; // test my struct
    //test_my_ptr<int>((void*)pRealStruct->FP_MF);

    uint32_t h[6] = {0, 1,
                     2, 3,
                     4, 5}; // should be matched with Img_t bitDepth!!
    // uint32_t h[5] = {0, 1, 1, 0, 0}; // should be matched with Img_t bitDepth!!
    const KernelCfg_t sKernelCfg = {
        (uint8_t*)h, 3, 2, 0, 1, ZEROPADDING, 1, 1, false, (void*)pRealStruct->FP_MF};
    // const KernelCfg_t sKernelCfg = {
    // (uint8_t*)h, 1, 5, 2, 0, ZEROPADDING, 1, 2, false};

    ROI_t sInImgROI = {0, 0, 0, width, height};
    ROI_t sOutImgROI = {0, 0, 0, width, height}; // TODO: may create a helper function to find ROI (???) based on kernel; 
    // TODO: create a helper function for out img width height and kernel step



    sliding_window(pImg1, sInImgROI, pImg2, sOutImgROI, sKernelCfg);

    std::cout<<"filtered:\n";
    ROI_t viewROI_2 = {0,0,0,width,height};
    view_image_data(pImg2, viewROI_2);

    destruct_img(&pImg1);
    destruct_img(&pImg2);
}
