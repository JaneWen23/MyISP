
#include <iostream>
#include <stdlib.h>
#include "ImgDef.hpp"
#include "../Math/Math.hpp"


const int get_next_multiple(const int x, const int m){
    // m is the multiple number (base number), and should be >= 1.
    // x is the number to find its next multiple of m
    assert(m > 0);
    assert(x > 0);
    if ((m & (m - 1)) == 0){
        // if m > 0 and m is power of 2, use this simplified algo.
        return ((x + m -1) & ~(m - 1));
    }
    else{       
        int q = 0;
        q = x%m;
        return x + m * (bool)q - q;
    }
}


const char* get_image_format_name(const IMAGE_FMT imageFormat){
    switch (imageFormat){
        case MONO:{
            return "MONO";
        }    
        case RGB:{
            return "RGB";
        }
        case RAW_RGGB:{
            return "RAW_RGGB";
        }
        case RAW_GRBG:{
            return "RAW_GRBG";
        }
        case RAW_GBRG:{
            return "RAW_GBRG";
        }
        case RAW_BGGR:{
            return "RAW_BGGR";
        }
        case YUV420:{
            return "YUV420";
        }
        case Y_C_C_D_RGGB:{
            return "Y_C_C_D_RGGB";
        }
        case Y_C_C_D_GRBG:{
            return "Y_C_C_D_GRBG";
        }
        case Y_C_C_D_GBRG:{
            return "Y_C_C_D_GBRG";
        }
        case Y_C_C_D_BGGR:{
            return "Y_C_C_D_BGGR";
        }
        default:{
            return "Unknown";
        }
    }
}

const IMAGE_FMT get_image_format_from_name(const char* name){
    // cannot use switch statement because it needs single integral value, non-array.
    if (strcmp(name, "MONO")==0){
        return MONO;
    }
    if (strcmp(name, "RGB")==0){
        return RGB;
    }
    if (strcmp(name, "RAW_RGGB")==0){
        return RAW_RGGB;
    }
    if (strcmp(name, "RAW_GRBG")==0){
        return RAW_GRBG;
    }
    if (strcmp(name, "RAW_GBRG")==0){
        return RAW_GBRG;
    }
    if (strcmp(name, "RAW_BGGR")==0){
        return RAW_BGGR;
    }
    if (strcmp(name, "YUV420")==0){
        return YUV420;
    }
    if (strcmp(name, "Y_C_C_D_RGGB")==0){
        return Y_C_C_D_RGGB;
    }
    if (strcmp(name, "Y_C_C_D_GRBG")==0){
        return Y_C_C_D_GRBG;
    }
    if (strcmp(name, "Y_C_C_D_GBRG")==0){
        return Y_C_C_D_GBRG;
    }
    if (strcmp(name, "Y_C_C_D_BGGR")==0){
        return Y_C_C_D_BGGR;
    }

    return UNKOWN;
}

const char* get_sign(const SIGN sign){
    if(sign == SIGNED){
        return "SIGNED";
    }
    else{
        return "UNSIGNED";
    }
}

void view_img_properties(const Img_t* pImg){
    if (pImg == NULL){
        std::cout<<"Cannot view because Img_t object is not allocated.\n";
        return;
    }
    std::cout<<"imageFormat = "<< get_image_format_name(pImg->imageFormat)<<"\n";
    std::cout<<"width = "<< pImg->width<<"\n";
    std::cout<<"height = "<< pImg->height<<"\n";
    std::cout<<"sign = "<< get_sign(pImg->sign)<<"\n";
    std::cout<<"bitDepth = "<< pImg->bitDepth<<"\n";
    std::cout<<"alignment = "<< pImg->alignment<<"\n";
    std::cout<<"strides = ";
    for (int c = 0 ; c < MAX_NUM_P; c++){
        std::cout<<pImg->strides[c]<<"; ";
    }
    std::cout<<"\n";
    std::cout<<"addr of pImageData = ";
    for (int c = 0 ; c < MAX_NUM_P; c++){
        if (pImg->pImageData[c] != NULL){
            std::cout<< static_cast<const void*>(pImg->pImageData[c]) <<"; ";
        }
        else{
            std::cout<<"NULL; ";
        }
    }
    std::cout<<"\n";
}

template<typename T>
void print_data_single(uint8_t* pData){
    std::cout<< + *((T*)(pData)); // "+" is to make output as digit, not as char
}

typedef void (*FP)(uint8_t*);

void view_image_data_planar(const Img_t* pImg, const ROI_t& sViewROI){
    assert(sViewROI.panelId < MAX_NUM_P);
    if(pImg->pImageData[sViewROI.panelId] == NULL){
        std::cout<<"error: the panel to view is not allocated yet. returned.\n";
        return;
    }

    FP f = NULL;
    int scale = 0;
    if (pImg->sign == UNSIGNED){
        if (pImg->bitDepth <= 8){
            f = print_data_single<uint8_t>;
            scale = sizeof(uint8_t);
        }
        else if (pImg->bitDepth <= 16){
            f = print_data_single<uint16_t>;
            scale = sizeof(uint16_t);
        }
        else if (pImg->bitDepth <= 32){
            f = print_data_single<uint32_t>;
            scale = sizeof(uint32_t);
        }
    }
    else {
        if (pImg->bitDepth <= 8){
            f = print_data_single<int8_t>;
            scale = sizeof(int8_t);
        }
        else if (pImg->bitDepth <= 16){
            f = print_data_single<int16_t>;
            scale = sizeof(int16_t);
        }
        else if (pImg->bitDepth <= 32){
            f = print_data_single<int>;
            scale = sizeof(int);
        }
    }

    uint8_t* pData = NULL; // addr of data
    std::cout<<"panel "<<sViewROI.panelId<<":\n";
    for (int i = sViewROI.startRow; i < sViewROI.startRow + sViewROI.roiHeight; ++i){
        pData = pImg->pImageData[sViewROI.panelId] + i*pImg->strides[sViewROI.panelId] + sViewROI.startCol * scale;
        for (int j = sViewROI.startCol; j < sViewROI.startCol + sViewROI.roiWidth; ++j){
            std::cout<<"\t";
            f(pData);
            pData += scale;
        }
        std::cout<<'\n';
    }
}

void view_image_data_packed(const Img_t* pImg, const ROI_t& sViewROI){
    assert(sViewROI.panelId >= MAX_NUM_P);
    assert(pImg->imageFormat == RGB); // currently only support RGB
    int cNum = 3; // currently set to 3 for RGB
    FP f = NULL;
    int scale = 0;
    if (pImg->sign == UNSIGNED){
        if (pImg->bitDepth <= 8){
            f = print_data_single<uint8_t>;
            scale = sizeof(uint8_t);
        }
        else if (pImg->bitDepth <= 16){
            f = print_data_single<uint16_t>;
            scale = sizeof(uint16_t);
        }
        else if (pImg->bitDepth <= 32){
            f = print_data_single<uint32_t>;
            scale = sizeof(uint32_t);
        }
    }
    else {
        if (pImg->bitDepth <= 8){
            f = print_data_single<int8_t>;
            scale = sizeof(int8_t);
        }
        else if (pImg->bitDepth <= 16){
            f = print_data_single<int16_t>;
            scale = sizeof(int16_t);
        }
        else if (pImg->bitDepth <= 32){
            f = print_data_single<int>;
            scale = sizeof(int);
        }
    }

    uint8_t* addr[MAX_NUM_P] = {NULL}; // addr of data
    for (int i = sViewROI.startRow; i < sViewROI.startRow + sViewROI.roiHeight; ++i){
        for (int c = 0; c < cNum; ++c){
            addr[c] = pImg->pImageData[c] + i * pImg->strides[c] + sViewROI.startCol * scale;
        }
        for (int j = sViewROI.startCol; j < sViewROI.startCol + sViewROI.roiWidth; ++j){
            std::cout<<"\t(";
            for (int c = 0; c < cNum; ++c){
                f(addr[c]);
                std::cout<<",";
            }
            std::cout<<")";
            for (int c = 0; c < cNum; ++c){
                addr[c] += scale;
            }
        }
        std::cout<<'\n';
    }
}

void view_image_data(const Img_t* pImg, const ROI_t& sViewROI){
    //check validity:
    if(sViewROI.startRow > pImg->height){
        std::cout<<"error: start_row to view is greater than image height. exited.\n";
        exit(1);
    }
    if(sViewROI.startCol > pImg->width){
        std::cout<<"error: start_column to view is greater than image width. exited.\n";
        exit(1);
    }
    if(sViewROI.startRow + sViewROI.roiHeight > pImg->height){
        std::cout<<"error: end_row to view excesses the image height. exited.\n";
        exit(1);
    }
    if(sViewROI.startCol + sViewROI.roiWidth > pImg->width){
        std::cout<<"error: end_column to view excesses the image width. exited.\n";
        exit(1);
    }

    if (sViewROI.panelId < MAX_NUM_P){
        view_image_data_planar(pImg, sViewROI);
        return;
    } 
    else {
        switch (pImg->imageFormat){
            case RGB:{
                assert(sViewROI.panelId == 1012);
                view_image_data_packed(pImg, sViewROI);
                break;
            }
            case YUV420:{
                assert(sViewROI.panelId == 1012);
                ROI_t sTmpROI = sViewROI;
                sTmpROI.panelId = 0;
                view_image_data_planar(pImg, sTmpROI);
                sTmpROI.panelId = 1;
                sTmpROI.startRow = sViewROI.startRow >> 1;
                sTmpROI.startCol = sViewROI.startCol >> 1;
                sTmpROI.roiHeight = sViewROI.roiHeight >> 1;
                sTmpROI.roiWidth = sViewROI.roiWidth >> 1;
                view_image_data_planar(pImg, sTmpROI);
                sTmpROI.panelId = 2;
                view_image_data_planar(pImg, sTmpROI);
                break;
            }
            default:{
                int cNum = 0;
                ROI_t sTmpROI = sViewROI;
                switch (sViewROI.panelId){
                    case 10123:{
                        cNum = 4;
                        break;
                    }
                    case 1012:{
                        cNum = 3;
                        break;
                    }
                    default:{
                        std::cout<<"error: currently the panelId is not supported. exited.\n";
                        exit(1);
                    }
                }
                for (int c = 0; c < cNum; ++c){
                    sTmpROI.panelId = c;
                    view_image_data_planar(pImg, sTmpROI);
                }
                break;
            }
        }
    }
}

const int get_num_of_bytes_per_pixel(int bitDepth){
    if (bitDepth < 1){
        std::cout<< "error: bit depth cannot be less than 1. exited.\n";
        exit(1);
    }
    if (bitDepth <= 8){
        return 1;
    }
    else if (bitDepth <= 16){
        return 2;
    }
    else if (bitDepth <= 32){
        return 4;
    }
    else{
        std::cout<< "the bit depth is not supported. exited.\n";
        exit(1);
    }
}

void set_strides(Img_t* pImg){
    // strides are initiated by zeros.
    for (int c = 0; c < MAX_NUM_P; c++){
        pImg->strides[c] = 0;
    }  

    // if panel_0 width actual is 100, while panel_1 actual width is 50, what will be the strides?
    // the width will be 100 for both panels, and it is only a number, it does not affect actual image allocation
    // so stride[0] = 100*num_of_bytes_for_a_pixel + make_up_alignment_bytes, 
    // and stride[1] = 50*num_of_bytes_for_a_pixel + make_up_alignment_bytes
    int bytes_per_line = get_next_multiple(pImg->width * get_num_of_bytes_per_pixel(pImg->bitDepth), pImg->alignment);
    switch (pImg->imageFormat){
        case MONO:
        case RAW_RGGB:
        case RAW_GRBG:
        case RAW_GBRG:
        case RAW_BGGR:{
            pImg->strides[0] = bytes_per_line;
            break;
        }
        case RGB:{
            pImg->strides[0] = pImg->strides[1] = pImg->strides[2] = bytes_per_line;
            break;
        }
        case Y_C_C_D_RGGB:
        case Y_C_C_D_GRBG:
        case Y_C_C_D_GBRG:
        case Y_C_C_D_BGGR:{
            pImg->strides[0] = pImg->strides[1] = pImg->strides[2] = pImg->strides[3] = bytes_per_line;
            break;
        }
        case YUV420:{
            pImg->strides[0] = bytes_per_line;
            pImg->strides[1] = pImg->strides[2] = get_next_multiple((pImg->width>>1) * get_num_of_bytes_per_pixel(pImg->bitDepth), pImg->alignment);
        }
        default:
            break;
    }
}   


IMG_RTN_CODE allocate_image_data(Img_t* pImg){
    // allocate memory for image data
    if (pImg == NULL){
        std::cout<<"the input arg pImg is null\n";
        return INVALID_INPUT;
    }
    switch(pImg->imageFormat){
        case MONO:
        case RAW_RGGB:
        case RAW_GRBG:
        case RAW_GBRG:
        case RAW_BGGR:{
            assert(pImg->pImageData[0] == NULL); // because we set it to NULL in construct_img().
            pImg->pImageData[0] = (uint8_t*)malloc(pImg->height * pImg->strides[0]);
            if (pImg->pImageData[0] == NULL){
                return ALLOCATION_FAIL;
            }
            break;
        }
        case RGB:{
            for (int c = 0; c < 3; c++){
                assert(pImg->pImageData[c] == NULL); // because we set it to NULL in construct_img().
                pImg->pImageData[c] = (uint8_t*)malloc(pImg->height * pImg->strides[c]);
                if (pImg->pImageData[c] == NULL){
                    return ALLOCATION_FAIL;
                }
            }
            break;
        }
        case Y_C_C_D_RGGB:
        case Y_C_C_D_GRBG:
        case Y_C_C_D_GBRG:
        case Y_C_C_D_BGGR:{
            for (int c = 0; c < 4; c++){
                assert(pImg->pImageData[c] == NULL); // because we set it to NULL in construct_img().
                pImg->pImageData[c] = (uint8_t*)malloc(pImg->height * pImg->strides[c]);
                if (pImg->pImageData[c] == NULL){
                    return ALLOCATION_FAIL;
                }
            }
            break;
        }
        case YUV420:{
            assert(pImg->pImageData[0] == NULL);
            assert(pImg->pImageData[1] == NULL);
            assert(pImg->pImageData[2] == NULL);
            pImg->pImageData[0] = (uint8_t*)malloc(pImg->height * pImg->strides[0]);
            //pImg->pImageData[1] = (uint8_t*)malloc(pImg->height * pImg->strides[1]);
            //pImg->pImageData[2] = (uint8_t*)malloc(pImg->height * pImg->strides[2]);
            pImg->pImageData[1] = (uint8_t*)malloc((pImg->height>>1) * pImg->strides[1]);
            pImg->pImageData[2] = (uint8_t*)malloc((pImg->height>>1) * pImg->strides[2]);
            for (int c = 0; c < 3; c++){
                if (pImg->pImageData[c] == NULL){
                    return ALLOCATION_FAIL;
                }
            }
            break;
        }
        default:
            return ALLOCATION_FAIL;
    }
    return SUCCEED;
}

IMG_RTN_CODE construct_img( Img_t* pImg, 
                            const IMAGE_FMT imageFormat,
                            const int width,
                            const int height,
                            const SIGN sign,
                            const int bitDepth,
                            const int alignment,
                            const bool allocateImage){
    // takes in args to set up the properties of the img type; 
    if (pImg == NULL){
        std::cout<<"error: Img_t object is not allocated.\n";
        return ALLOCATION_FAIL;
    }
    pImg->imageFormat = imageFormat;
    pImg->width = width; // but there may be a different width and/or height in a different channel, what we do is to control the stride
    pImg->height = height;
    pImg->sign = sign;
    pImg->bitDepth = bitDepth;
    pImg->alignment = alignment;
 
    if (alignment == 0){
        std::cout<<" error: alignment should be >= 1 but got 0 instead.\n";
        return INVALID_INPUT;
    }
    set_strides(pImg);
    for (int c = 0; c < MAX_NUM_P; c++){
        pImg->pImageData[c] = NULL;
    }  
    if (allocateImage){
        if (allocate_image_data(pImg) != SUCCEED){
            std::cout<<"error: image is not successfully allocated.\n";
            return ALLOCATION_FAIL;
        }
    }
    return SUCCEED;
}

void find_heights_or_widths(const Img_t* pImg, char option, int* values){
    // option = 'h', means to find heights
    // option = 'w', means to find widths
    int tmp = 0;
    switch (option){
        case 'h':{
            tmp = pImg->height;
            break;
        }
        case 'w':{
            tmp = pImg->width;
            break;
        }
    }
    switch (pImg->imageFormat){
        case MONO:
        case RGB:
        case RAW_RGGB:
        case RAW_GRBG:
        case RAW_GBRG:
        case RAW_BGGR:
        case Y_C_C_D_RGGB:
        case Y_C_C_D_GRBG:
        case Y_C_C_D_GBRG:
        case Y_C_C_D_BGGR:{
            for (int c = 0; c < MAX_NUM_P; ++c){
                values[c] = tmp; // if there are less than MAX_NUM_P panels, just do not use.
            }
            break;
        }
        case YUV420:{
            values[0] = tmp;
            values[1] = values[2] = tmp >> 1;
            break;
        }
        default:{
            std::cout<<"error: image format is not supported. exited.\n";
            exit(1);
        }
    }
}

IMG_RTN_CODE duplicate_img(const Img_t* pSrcImg, Img_t* pDstImg){
    // dose NOT set ROI because ROI size is in pixel (each pixel may take 1, 2 or 4 bytes), 
    // but the Img_t cannot handle different data types.
    if (pSrcImg == NULL){
        std::cout<<"error: source Img_t object is not allocated.\n";
        return ALLOCATION_FAIL;
    }
    if (pSrcImg->pImageData[0] == NULL){
        std::cout<<"error: source image is not reachable, maybe it is not allocated.\n";
        return ALLOCATION_FAIL;
    }
    if (pDstImg == NULL){
        std::cout<<"error: destination Img_t object is not allocated.\n";
        return ALLOCATION_FAIL;
    }
    if (construct_img(pDstImg, 
                    pSrcImg->imageFormat, 
                    pSrcImg->width,
                    pSrcImg->height,
                    pSrcImg->sign,
                    pSrcImg->bitDepth,
                    pSrcImg->alignment,
                    true) == SUCCEED){
        int heights[MAX_NUM_P] = {0};
        find_heights_or_widths(pSrcImg, 'h', heights);
        for (int c = 0; c < MAX_NUM_P; ++c){
            if (pSrcImg->pImageData[c] != NULL){
                for (int i = 0; i < heights[c]; ++i){
                    for (int j = 0; j < pSrcImg->strides[c]; ++j){
                        *(pDstImg->pImageData[c] + i * (pSrcImg->strides[c]) + j) = *(pSrcImg->pImageData[c] + i * (pSrcImg->strides[c]) + j);
                    }
                }
            }
        }
    }
    else{
        std::cout<<"error: destination image not allocated successfully.\n";
        return ALLOCATION_FAIL;
    }
    return SUCCEED;
}

// TODO: crop_image()

void print_clamp_warning(const int a, const int b){
    if (a != b){
        std::cout<<"warning: value clipped, from " << a << " to "<< b <<"\n";
    }
}

void do_not_print_clamp_waring(const int a, const int b){
    // do nothing
}

typedef void (*FPRINTCLAMP)(int, int);

template <typename T> 
void clamp_img_with_type(Img_t* pImg, int lowerBound, int upperBound, bool printWarning){
    Clamp_T<T> Fclamp;
    Fclamp.f = clamp;
    FPRINTCLAMP f_print = NULL;
    if (printWarning){
        f_print = print_clamp_warning;
    }
    else{
        f_print = do_not_print_clamp_waring;
    }
    int heights[MAX_NUM_P] = {0};
    int widths[MAX_NUM_P] = {0};
    find_heights_or_widths(pImg, 'h', heights);
    find_heights_or_widths(pImg, 'w', widths);
    T tmp = 0;
    T pixVal = 0;
    for (int c = 0; c < MAX_NUM_P; ++c){
        uint8_t* pRow = pImg->pImageData[c];
        if (pImg->pImageData[c] != NULL){
            for (int i = 0; i < heights[c]; ++i){
                for (int j = 0; j < widths[c]; ++j){
                    pixVal = *((T*)pRow + j);
                    tmp = Fclamp.f(pixVal, (T)lowerBound, (T)upperBound);
                    f_print(pixVal, tmp);
                    *((T*)pRow + j) = tmp;
                }
                pRow += pImg->strides[c];
            }
        }
    }
}

IMG_RTN_CODE clamp_img(Img_t* pImg, int lowerBound, int upperBound, bool printWarning){
    if (pImg->sign == SIGNED){
        if (pImg->bitDepth <= 8){
            clamp_img_with_type<int8_t>(pImg, lowerBound, upperBound, printWarning);
            return SUCCEED;
        }
        else if (pImg->bitDepth <= 16){
            clamp_img_with_type<int16_t>(pImg, lowerBound, upperBound, printWarning);
            return SUCCEED;
        }
        else if (pImg->bitDepth <= 32){
            clamp_img_with_type<int>(pImg, lowerBound, upperBound, printWarning);
            return SUCCEED;
        }
    }
    else{
        if (pImg->bitDepth <= 8){
            clamp_img_with_type<uint8_t>(pImg, lowerBound, upperBound, printWarning);
            return SUCCEED;
        }
        else if (pImg->bitDepth <= 16){
            clamp_img_with_type<uint16_t>(pImg, lowerBound, upperBound, printWarning);
            return SUCCEED;
        }
        else if (pImg->bitDepth <= 32){
            clamp_img_with_type<uint32_t>(pImg, lowerBound, upperBound, printWarning);
            return SUCCEED;
        }
    }
    return PROCESS_INCOMPLETE;
}

template<typename Tsrc, typename Tdst>
void copy_pix_with_type(uint8_t* pSrc, uint8_t* pDst){
    Tsrc tmp = *((Tsrc*)pSrc);
    //*(Tdst*)pDst = static_cast<Tdst>(tmp);
    *(Tdst*)pDst = (Tdst)tmp;
}
typedef void (*FCOPYPIX)(uint8_t*, uint8_t*);

template<typename Tsrc>
FCOPYPIX choose_copy_pix_func(int dstTypeBytes){
    switch (dstTypeBytes){
        case 1:{
            return copy_pix_with_type<Tsrc, uint8_t>;
        }
        case 2:{
            return copy_pix_with_type<Tsrc, uint16_t>;
        }
        case 4:{
            return copy_pix_with_type<Tsrc, uint32_t>;
        } // no problem to use unsigned types, because only bit depth matters
        default:{
            std::cout<<"error: choose_copy_pix_func(): data type not supported. exited.\n";
        }
    }
    return NULL;
}

IMG_RTN_CODE change_img_bitDepth(Img_t* pImg, const int dstBitDepth){
    if (pImg == NULL){
        std::cout<<"error: source Img_t object is not allocated.\n";
        return INVALID_INPUT;
    }

    // do not discuss sign. only discuss bit depth. because data are stored in uint8.

    // first discuss whether need to change data type or not,
    // if not, just change pImg->bitDepth, done.
    // if need to change data type:
    // re-allocate the imageData, then copy the data, then free original imageData

    int srcBitDepth = pImg->bitDepth;
    int srcTypeBytes = get_num_of_bytes_per_pixel(srcBitDepth);
    int dstTypeBytes = get_num_of_bytes_per_pixel(dstBitDepth);
    if (srcTypeBytes == dstTypeBytes){
        pImg->bitDepth = dstBitDepth;
        return SUCCEED;
    }
    else{
        uint8_t* pSrcImageData[MAX_NUM_P] = {NULL};
        int srcStrides[MAX_NUM_P] = {0};
        for (int c = 0; c < MAX_NUM_P; ++c){
            pSrcImageData[c] = pImg->pImageData[c];
            srcStrides[c] = pImg->strides[c];
            pImg->pImageData[c] = NULL;
            pImg->strides[c] = 0;
        }
        pImg->bitDepth = dstBitDepth;
        set_strides(pImg);
        allocate_image_data(pImg);
        int heights[MAX_NUM_P] = {0};
        find_heights_or_widths(pImg, 'h', heights); // dst heights
        int widths[MAX_NUM_P] = {0};
        find_heights_or_widths(pImg, 'w', widths); // dst widths
        
        FCOPYPIX f = NULL;
        switch (srcTypeBytes){
            case 1:{
                f = choose_copy_pix_func<uint8_t>(dstTypeBytes);
                break;
            }
            case 2:{
                f = choose_copy_pix_func<uint16_t>(dstTypeBytes);
                break;
            }
            case 4:{
                f = choose_copy_pix_func<uint32_t>(dstTypeBytes);
                break;
            }
        }
        
        for (int c = 0; c < MAX_NUM_P; ++c){
            uint8_t* pDstRow = pImg->pImageData[c];
            uint8_t* pSrcRow = pSrcImageData[c];
            if (pImg->pImageData[c] != NULL){
                for (int i = 0; i < heights[c]; ++i){
                    int jj = 0;
                    for (int j = 0; j < widths[c] * dstTypeBytes; j += dstTypeBytes){
                        f(pSrcRow + jj, pDstRow + j);
                        jj += srcTypeBytes;
                    }
                    pDstRow += pImg->strides[c];
                    pSrcRow += srcStrides[c];
                }
            }
        }
        for (int c = 0; c < MAX_NUM_P; ++c){
            if(pSrcImageData[c] != NULL){
                free(pSrcImageData[c]);
            }
        }
    }

    return SUCCEED;
}

IMG_RTN_CODE safe_signed_to_unsigned_img(Img_t* pImg){
    if (pImg->sign != SIGNED){
        std::cout<<"warning: called signed_to_unsigned_img() but input img is UNSIGNED.\n";
        return INVALID_INPUT;
    }
    // usually used when module finished processing an image,
    // since the processed image may be signed but pipeline only allows
    // data in range [0, 2^bitDepth - 1], we should first set negatives to 0, then set sign to UNSIGNED.
    // set negative values to 0: if it really happens, print a warning.

    int dstBitDepth = pImg->bitDepth - 1;
    clamp_img(pImg, 0, (1 << (dstBitDepth)) - 1, true);
    change_img_bitDepth(pImg, dstBitDepth);
    
    pImg->sign = UNSIGNED;
    return SUCCEED;
}

IMG_RTN_CODE safe_unsigned_to_signed_img(Img_t* pImg){
    if (pImg->sign != UNSIGNED){
        std::cout<<"warning: called unsigned_to_signed_img() but input img is SIGNED.\n";
        return INVALID_INPUT;
    }
    // this is usually used when module just got the img from pipeline,
    // in the case that the algo needs signed img, 
    // module should convert it to SIGNED before pass to algo.

    // for example:
    // if sensor bit depth is 12, we need bitDepth >= 13 to make sure signed data type also carries "full" data.

    int dstBitDepth = pImg->bitDepth + 1;
    change_img_bitDepth(pImg, dstBitDepth);

    pImg->sign = SIGNED;
    return SUCCEED;
}

bool is_image_equal(const Img_t* pSrcImg, const Img_t* pDstImg){ // TODO: add option to compare "visible" only
    for (int c = 0; c < MAX_NUM_P; ++c){
        if (pSrcImg->pImageData[0] != NULL){
            for (int i = 0; i < pSrcImg->height; ++i){
                for (int j = 0; j < pSrcImg->strides[c]; ++j){
                    if(*(pDstImg->pImageData[c] + i * (pSrcImg->strides[c]) + j) != *(pSrcImg->pImageData[c] + i * (pSrcImg->strides[c]) + j)){
                        std::cout<<"not equal: at location c, i, j(bytes) = "<<c<<", "<<i<<", "<<j<<"\n";
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

IMG_RTN_CODE free_image_data(Img_t* pImg){
    for (int c = 0; c < MAX_NUM_P; c++){
        if (pImg->pImageData[c] != NULL){
            free(pImg->pImageData[c]);
            pImg->pImageData[c] = NULL;
        }
    }
    return SUCCEED; // always succeed?
}

IMG_RTN_CODE destruct_img(Img_t** ptr_pImg){
    // this is different from "free_image_data(Img_t* pImg)", because to free the "image
    // pointer" needs to find its address taking use of pImg, and we do not change pImg.
    // when we want to free pImg itself, we want to free the argument, not only the parameter.
    // so we need another pointer(i.e., pointer to pointer) to "reflect to" the address of pImg.
    assert(ptr_pImg != NULL);
    free_image_data(*ptr_pImg);
    free(*ptr_pImg);
    *ptr_pImg = NULL;
    return SUCCEED; // always succeed?
}



void test_img_def(){

    Img_t* pMyImg = NULL; // initialize
    pMyImg =(Img_t*)malloc(sizeof(Img_t));
    IMAGE_FMT imageFormat = RGB;
    int width = 5;
    int height = 6;
    int bitDepth = 9;
    int alignment = 16;
    if (construct_img(pMyImg, 
                imageFormat,
                width,
                height,
                UNSIGNED,
                bitDepth,
                alignment,
                true) == SUCCEED) {
        std::cout<<"ok\n";
    }

    for (int c = 0; c < 3; ++c){
        uint8_t* pRow = pMyImg->pImageData[c];
        for (int i = 0; i < height; ++i){
            for (int j = 0; j < pMyImg->strides[c]; j += get_num_of_bytes_per_pixel(bitDepth)){
                *(uint32_t*)(pRow + j) = 0;
            }
            pRow += pMyImg->strides[c];
        }
    }

    view_img_properties(pMyImg);
    ROI_t roi = {1012, 0, 0, pMyImg->width, pMyImg->height};
    view_image_data(pMyImg, roi);

    clamp_img(pMyImg, 3, 254, true);
    std::cout<<"clamped: \n";
    view_image_data(pMyImg, roi);

    int dstBitDepth = 24;
    change_img_bitDepth(pMyImg, dstBitDepth);
    view_img_properties(pMyImg);
    view_image_data(pMyImg, roi);

    destruct_img(&pMyImg);
 }