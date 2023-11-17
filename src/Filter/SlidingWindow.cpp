
#include "SlidingWindow.hpp"
#include "../Math/Math.hpp"

// sliding window: a uniform API to perform 1D/2D filtering of any kernel, with configurable settings.


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
void addr_matrix_transpose(T** pSrc, const int srcHeight, const int srcWidth, T** pDst){ // not in use
    for (int i = 0; i < srcHeight; ++i){
        for (int j = 0; j < srcWidth; ++j){
            *(pDst + j*srcHeight + i) = *(pSrc + i*srcWidth + j);
        }
    }
    // or, for i = 0, 1, ..., L-1:
    // mapped(i) = (i % srcHeight) * srcWidth + i
    // mapped(i) is the index of pDst to be set value of pSrc[i].
}


template<typename T>
void copy_row_to_col(T** pSrc, const int srcRowInd, const int srcHeight, const int srcWidth, T** pDst){ // not in use
    // matrix transpose.
    // src and dst are BOTH matrices, but only need one row of src matrix at a time.
    // copy each src row to the col of destination.

    for (int j = 0; j < srcWidth; ++j){
        *(pDst + j*srcHeight + srcRowInd) = *(pSrc + j);
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
void filter_2d_divide_conquer_unit(const T** px, const int xWidth, 
                  const T** pKerAddrMatrix, 
                  const KernelCfg_t& sKernelCfg, 
                  const int xOutWidth, T* y){
    // px[i] stored the head address of i-th line of x, i is in terms of kernel height
    // outside this func: need augmented "x", i.e., pad the non-existed rows;
    // inside this func: pad the x horizontally, stack this operation for all #_kernel_height lines
   
    // kerWidth: to help with horizontal padding len
    // kerHeight: to determine how many rows needed for the 2d convolution to output one line
    int xWidthPadded = xWidth + sKernelCfg.kerWidth - 1;
    T yTmp = 0;
    T** pAddrMatrix = (T**)malloc(sKernelCfg.kerHeight * xWidthPadded * sizeof(T*));
    const T z = 0; // useful for zero-padding
    Formulas_T<T> Formula;
    Formula.f = (decltype(Formula.f))(sKernelCfg.formula); // convert void* to the type of Formula.f
    
    for (int i = 0; i < sKernelCfg.kerHeight; ++i){
        set_addr_array<T>(pAddrMatrix+i*xWidthPadded, px[i], xWidth, xWidthPadded, sKernelCfg.horiCenter, sKernelCfg.padding, &z);
    }

    int jj = 0; // horizontal index at the output image
    for (int j = 0; j < xOutWidth; j += sKernelCfg.horiStep){
        // divide and conquer: "2d dot product" is the sum of several 1d dot products (because of associativity);
        yTmp = 0; // this is helpful for the manipulation of 1d results.
        for (int i = 0; i < sKernelCfg.kerHeight; ++i){
            yTmp += Formula.f((const T **)pAddrMatrix + j + i*xWidthPadded, (const T **)pKerAddrMatrix+i*sKernelCfg.kerWidth, sKernelCfg.kerWidth);
            // i.e. yTmp = manipulate(yTmp, Formula.f(...)); 
            // currently, the manipulation is addition. theoretically it can also be multiplication. 
            // but it seems that multiplication is not quite used, so currently we do not consider multiplication.
        }
        *(y + jj) = yTmp;
        jj += sKernelCfg.horiUpsample;
    }
    free(pAddrMatrix);
}

template<typename T>
void filter_2d_flatten_unit(const T** px, const int xWidth, 
                  const T** pKerAddrMatrix, 
                  const KernelCfg_t& sKernelCfg, 
                  const int xOutWidth, T* y){
    int xWidthPadded = xWidth + sKernelCfg.kerWidth - 1;
    T yTmp = 0;
    T** pAddrMatrix = (T**)malloc(sKernelCfg.kerHeight * xWidthPadded * sizeof(T*));
    T** pAddrFlatten = (T**)malloc(sKernelCfg.kerHeight * sKernelCfg.kerWidth * sizeof(T*));
    const T z = 0; // useful for zero-padding
    Formulas_T<T> Formula;
    Formula.f = (decltype(Formula.f))(sKernelCfg.formula); // convert void* to the type of Formula.f
    
    for (int i = 0; i < sKernelCfg.kerHeight; ++i){
        set_addr_array<T>(pAddrMatrix + i * xWidthPadded, px[i], xWidth, xWidthPadded, sKernelCfg.horiCenter, sKernelCfg.padding, &z);
    }

    int jj = 0; // horizontal index at the output image
    for (int j = 0; j < xOutWidth; j += sKernelCfg.horiStep){
        // prepare the addresses
        for(int l = 0; l < sKernelCfg.kerHeight; ++l){
            for (int k = 0; k < sKernelCfg.kerWidth; ++k){
                *(pAddrFlatten + l * sKernelCfg.kerWidth + k) = *(pAddrMatrix + l * xWidthPadded + j + k);
            }
        }
        // apply the flatten kernel
        *(y + jj) += Formula.f((const T **)pAddrFlatten, (const T **)pKerAddrMatrix, sKernelCfg.kerHeight*sKernelCfg.kerWidth);
        jj += sKernelCfg.horiUpsample;
    }
    free(pAddrMatrix);
    free(pAddrFlatten);
}

template<typename T>
struct Unit2D_T{
    void (*filt_2d_unit)(const T**, const int, const T**, const KernelCfg_t&, const int, T*);
}; // note: cannot typedef, because there is no type yet.

template<typename T>
void filter_2d(const uint8_t* x, const int xWidth, const int xHeight,
             const T** pKerAddrMatrix, const KernelCfg_t& sKernelCfg, 
             const int inImgStride, const int outImgStride, 
             const int xOutWidth, uint8_t* y){
    uint8_t** px = (uint8_t**)malloc(sKernelCfg.kerHeight * sizeof(T*));
    uint8_t* zVec = (uint8_t*)malloc(xWidth * sizeof(T));
    memset(zVec, 0, xWidth * sizeof(T));
    Unit2D_T<T> UnitImplement;

    if (sKernelCfg.divideAndConquer){
        UnitImplement.filt_2d_unit = (decltype(UnitImplement.filt_2d_unit))filter_2d_divide_conquer_unit;
    }
    else{
        UnitImplement.filt_2d_unit = (decltype(UnitImplement.filt_2d_unit))filter_2d_flatten_unit;
    }

    for (int i = 0; i < xHeight; i += sKernelCfg.vertStep){
        for (int l = -sKernelCfg.vertCenter; l < sKernelCfg.kerHeight - sKernelCfg.vertCenter; ++l){
            px[l + sKernelCfg.vertCenter] = (uint8_t*)vert_padding_map(i + l, x, xHeight, inImgStride, sKernelCfg.padding, (const uint8_t*)zVec);
        }
        
        UnitImplement.filt_2d_unit((const T**)px, xWidth, 
                        pKerAddrMatrix, 
                        sKernelCfg,
                        xOutWidth, (T*)y);

        y += sKernelCfg.vertUpsample * outImgStride;
    }
    free(px);
    free(zVec);
}


template<typename T>
void filter_1d_vertical_unit(const T** px, const int xRoiWidth, 
                           const T** pKerAddr, 
                           const KernelCfg_t& sKernelCfg, 
                           T* y){
    Formulas_T<T> Formula;
    Formula.f = (decltype(Formula.f))(sKernelCfg.formula); // convert void* to the type of Formula.f
    int jj = 0; // horizontal index at the output image
    for(int j = 0; j < xRoiWidth; j += sKernelCfg.horiStep){
        *(y + jj) = Formula.f((const T **)px, (const T **)pKerAddr, sKernelCfg.kerHeight);
        jj += sKernelCfg.horiUpsample;
        for(int i = 0; i < sKernelCfg.kerHeight; ++i){
            px[i] += 1;
        }
    }
}

template<typename T>
void filter_1d_vertical(const uint8_t* x, const int xWidth, const int xHeight,
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
        
        filter_1d_vertical_unit<T>((const T**)px, xWidth, 
                        pKerAddrMatrix, 
                        sKernelCfg,
                        (T*)y);

        y += sKernelCfg.vertUpsample * outImgStride;
    }
    free(px);
    free(zVec);
}

template<typename T>
void filter_1d_horizontal_unit(const T** px, const int xWidth, 
                             const T** pKerAddr, 
                             const KernelCfg_t& sKernelCfg,  
                             const int xOutWidth, T* y){
    int xWidthPadded = xWidth + sKernelCfg.kerWidth - 1;
    T** pAddrArray = (T**)malloc(xWidthPadded * sizeof(T*)); // stores the addr of padded x
    const T z = 0; // this is useful for zero-padding
    Formulas_T<T> Formula;
    Formula.f = (decltype(Formula.f))(sKernelCfg.formula); // convert void* to the type of Formula.f

    set_addr_array<T>(pAddrArray, px[0], xWidth, xWidthPadded, sKernelCfg.horiCenter, sKernelCfg.padding, &z);

    int jj = 0; // horizontal index at the output image
    for(int j = 0; j < xOutWidth; j += sKernelCfg.horiStep){
        *(y + jj) = Formula.f((const T **)pAddrArray + j, (const T **)pKerAddr, sKernelCfg.kerWidth);
        jj += sKernelCfg.horiUpsample;
    }
    free(pAddrArray);
}

template<typename T>
void filter_1d_horizontal(const uint8_t* x, const int xWidth, const int xHeight,
             const T** pKerAddrMatrix, const KernelCfg_t& sKernelCfg, 
             const int inImgStride, const int outImgStride, 
             const int xOutWidth, uint8_t* y){
    for (int i = 0; i < xHeight; i += sKernelCfg.vertStep){
        filter_1d_horizontal_unit<T>((const T**)(&x), xWidth, 
                        pKerAddrMatrix, 
                        sKernelCfg,
                        xOutWidth, (T*)y);
        x += sKernelCfg.vertStep * inImgStride;
        y += sKernelCfg.vertUpsample * outImgStride;
    }
}

template<typename T>
void filter(const uint8_t* x, const int inImgStride, const int inImgRoiWidth, const int inImgRoiHeight,
                  const KernelCfg_t& sKernelCfg,
                  const int outImgStride,
                  const int outImgRoiWidth, uint8_t* y){
    assert(sKernelCfg.kerHeight > 0);
    assert(sKernelCfg.kerWidth > 0);
    T** pKerAddrMatrix = (T**)malloc(sKernelCfg.kerHeight * sKernelCfg.kerWidth * sizeof(T*)); // stores the addr of flipped (or not) kernel;
    if (sKernelCfg.pKernel != NULL){
        set_kernel_addr<T>(pKerAddrMatrix, (T*)sKernelCfg.pKernel, sKernelCfg.kerHeight * sKernelCfg.kerWidth, sKernelCfg.needFlip);
    }
    
    if (sKernelCfg.kerHeight == 1){
        filter_1d_horizontal<T>(x, inImgRoiWidth, inImgRoiHeight,
            (const T**)pKerAddrMatrix, sKernelCfg, 
            inImgStride, outImgStride, 
            outImgRoiWidth, y);
    }
    else if (sKernelCfg.kerWidth == 1){
        filter_1d_vertical<T>(x, outImgRoiWidth, inImgRoiHeight,
            (const T**)pKerAddrMatrix, sKernelCfg, 
            inImgStride, outImgStride, 
            y); // note that the second argument is different, and total argument number is different
    }
    else{
        filter_2d<T>(x, inImgRoiWidth, inImgRoiHeight,
            (const T**)pKerAddrMatrix, sKernelCfg, 
            inImgStride, outImgStride, 
            outImgRoiWidth, y);
    }
    free(pKerAddrMatrix);
}

typedef void (*FP_FILT)(const uint8_t*, const int, const int, const int,
                        const KernelCfg_t&,const int, const int, uint8_t*);

IMG_RTN_CODE sliding_window(const Img_t* pInImg, const ROI_t& sInImgROI, Img_t* pOutImg, const ROI_t& sOutImgROI, const KernelCfg_t& sKernelCfg){
    assert(pInImg != NULL);
    assert(pOutImg != NULL);
    int inImgStride = pInImg->strides[sInImgROI.panelId];
    int outImgStride = pOutImg->strides[sOutImgROI.panelId];
    int scale = 0;
    FP_FILT f = NULL;

    if (pOutImg->sign == UNSIGNED){
        if (pOutImg->bitDepth <= 8){
            scale = sizeof(uint8_t);
            f = filter<uint8_t>;
        }
        else if (pOutImg->bitDepth <= 16){
            scale = sizeof(uint16_t);
            f = filter<uint16_t>;
        }
        else if (pOutImg->bitDepth <= 32){
            scale = sizeof(uint32_t);
            f = filter<uint32_t>;
        }
    }
    else{
        if (pOutImg->bitDepth <= 8){
            scale = sizeof(int8_t);
            f = filter<int8_t>;
        }
        else if (pOutImg->bitDepth <= 16){
            scale = sizeof(int16_t);
            f = filter<int16_t>;
        }
        else if (pOutImg->bitDepth <= 32){
            scale = sizeof(int);
            f = filter<int>;
        }
    }

    assert(scale > 0);

    const uint8_t* x = pInImg->pImageData[sInImgROI.panelId] + sInImgROI.startRow * inImgStride + sInImgROI.startCol * scale;
    uint8_t* y = pOutImg->pImageData[sOutImgROI.panelId]+ sOutImgROI.startRow * outImgStride + sOutImgROI.startCol * scale;

    f(x, inImgStride, sInImgROI.roiWidth, sInImgROI.roiHeight,
      sKernelCfg,
      outImgStride,
      sOutImgROI.roiWidth, y);
    
    return SUCCEED;
}



void test_sliding_window(){

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

    Formulas_T<int> MyFml;
    MyFml.f = dot_product;

    uint32_t h[6] = {0, 0,
                     0, 1,
                     0, 1}; // should be matched with Img_t bitDepth!!
    //uint32_t h[5] = {0, 1, 1, 0, 0}; // should be matched with Img_t bitDepth!!
    const KernelCfg_t sKernelCfg = {
        (uint8_t*)h, 3, 2, 1, 1, ZEROPADDING, 1, 1, 1, 1, false, (void*)MyFml.f, false};
    // const KernelCfg_t sKernelCfg = {
    // (uint8_t*)h, 1, 5, 2, 0, ZEROPADDING, 1, 1, 1, 1, false, (void*)pMyFml.f, true};

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
