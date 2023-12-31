#include "OpenCV.hpp"
#include "../Algos/Infra/RandImageGen.hpp"
#include "../Algos/Infra/ImageIO.hpp"

using namespace cv;


void construct_img_t_according_to_cv_mat(Img_t* pImg, const int alignment, const cv::Mat& image, bool needOverride = false, SIGN dstSign = UNSIGNED, int dstBitDepth = 0){
    IMAGE_FMT imageFormat = UNKOWN;
    int width = image.cols;
    int height = image.rows;
    int bitDepth = 0;
    SIGN sign = SIGNED;
    switch (image.type()){
        case CV_8UC1:{
            imageFormat = MONO;
            bitDepth = 8;
            sign = UNSIGNED;
            break;
        }
        case CV_8UC2:{
            break;
        }
        case CV_8UC3:{
            imageFormat = RGB;
            bitDepth = 8;
            sign = UNSIGNED;
            break;
        }
        case CV_8UC4:{
            break;
        }
        case CV_8SC1:{
            imageFormat = MONO;
            bitDepth = 8;
            sign = SIGNED;
            break;
        }
        case CV_8SC2:{
            break;
        }
        case CV_8SC3:{
            imageFormat = RGB;
            bitDepth = 8;
            sign = SIGNED;
            break;
        }
        case CV_8SC4:{
            break;
        }
        case CV_16UC1:{
            imageFormat = MONO;
            bitDepth = 16;
            sign = UNSIGNED;
            break;
        }
        case CV_16UC2:{
            break;
        }
        case CV_16UC3:{
            imageFormat = RGB;
            bitDepth = 16;
            sign = UNSIGNED;
            break;
        }
        case CV_16UC4:{
            break;
        }
        case CV_16SC1:{
            imageFormat = MONO;
            bitDepth = 16;
            sign = SIGNED;
            break;
        }
        case CV_16SC2:{
            break;
        }
        case CV_16SC3:{
            imageFormat = RGB;
            bitDepth = 16;
            sign = SIGNED;
            break;
        }
        case CV_16SC4:{
            break;
        }
        case CV_32SC1:{
            imageFormat = MONO;
            bitDepth = 32;
            sign = SIGNED;
            break;
        }
        case CV_32SC2:{
            break;
        }
        case CV_32SC3:{
            imageFormat = RGB;
            bitDepth = 32;
            sign = SIGNED;
            break;
        }
        case CV_32SC4:{
            break;
        }
        default:{
            break;
        }
    }
    if (needOverride){
        sign = dstSign;
        bitDepth = dstBitDepth;
    }
    construct_img(pImg, imageFormat, width, height, sign, bitDepth, alignment, true);
}

template<typename Tsrc, typename Tdst>
void copy_data_from_cv_mat_to_img_t(Img_t* pImg, cv::Mat& image){
    assert(pImg != NULL);
    assert(pImg->pImageData != NULL);
    if (pImg->imageFormat == RGB){
        for (int i = 0; i < image.rows; ++i){
            Tsrc* pCvRow = image.ptr<Tsrc>(i);
            for (int j = 0; j < image.cols; ++j){
                for (int c = 0; c < 3; ++c){
                    *((Tdst*)(pImg->pImageData[2-c] + i * pImg->strides[2-c]) + j) = *(pCvRow + j*3 + c); // cv mat is in BGR
                }
            }
        }
    }
    else if (pImg->imageFormat == MONO){
        for (int i = 0; i < image.rows; ++i){
            Tsrc* pCvRow = image.ptr<Tsrc>(i);
            for (int j = 0; j < image.cols; ++j){
                *((Tdst*)(pImg->pImageData[0] + i * pImg->strides[0]) + j) = *(pCvRow + j);
            }
        }
    }
}
typedef void (*FCOPY)(Img_t*, cv::Mat&);

template<typename Tdst>
void copy_data_to_img_t(Img_t* pImg, cv::Mat& image){
    FCOPY f = NULL;
    switch (image.type()) {
        case CV_8UC1:
        case CV_8UC3:{
            f = copy_data_from_cv_mat_to_img_t<uint8_t, Tdst>;
            break;
        }
        case CV_8SC1:
        case CV_8SC3:{
            f = copy_data_from_cv_mat_to_img_t<int8_t, Tdst>;
            break;
        }
        case CV_16UC1:
        case CV_16UC3:{
            f = copy_data_from_cv_mat_to_img_t<uint16_t, Tdst>;
            break;
        }
        case CV_16SC1:
        case CV_16SC3:{
            f = copy_data_from_cv_mat_to_img_t<int16_t, Tdst>;
            break;
        }
        case CV_32SC1:
        case CV_32SC3:{
            f = copy_data_from_cv_mat_to_img_t<int, Tdst>;
            break;
        }
        default:{
            break;
        }
    }
    assert(f != NULL);
    f(pImg, image);
}

typedef void (*FOPT)(Img_t*, cv::Mat&);

void convert_cv_mat_to_img_t(cv::Mat& image, Img_t* pImg, const int alignment, bool needOverride, SIGN dstSign, int dstBitDepth){
    assert(pImg != NULL);
    construct_img_t_according_to_cv_mat(pImg, alignment, image, needOverride, dstSign, dstBitDepth);
    FOPT g = NULL;
    if (pImg->sign == UNSIGNED){
        if (pImg->bitDepth <= 8){
            g = copy_data_to_img_t<uint8_t>;
        }
        else if (pImg->bitDepth <= 16){
            g = copy_data_to_img_t<uint16_t>;
        }
        else if (pImg->bitDepth <= 32){
            g = copy_data_to_img_t<uint32_t>;
        }
    }
    else{
        if (pImg->bitDepth <= 8){
            g = copy_data_to_img_t<int8_t>;
        }
        else if (pImg->bitDepth <= 16){
            g = copy_data_to_img_t<int16_t>;
        }
        else if (pImg->bitDepth <= 32){
            g = copy_data_to_img_t<int>;
        }
    }
    assert(g != NULL);
    g(pImg, image);
}

template<typename T>
void copy_data_from_img_t_to_cv_mat(cv::Mat& image, const Img_t* pImg, const int panelId = 0){ // panelId is needed only in some cases
    assert(pImg != NULL);
    assert(pImg->pImageData[0] != NULL);
    switch (pImg->imageFormat){
        case RGB:{
            for (int i = 0; i < image.rows; ++i){
                T* pCvRow = image.ptr<T>(i);
                for (int j = 0; j < image.cols; ++j){
                    for (int c = 0; c < 3; ++c){
                        *(pCvRow + j*3 + c) = *((T*)(pImg->pImageData[2-c] + i * pImg->strides[2-c]) + j); // cv mat is in BGR
                    }
                }
            }
            break;
        }
        case RAW_RGGB:
        case RAW_GRBG:
        case RAW_GBRG:
        case RAW_BGGR:
        case MONO:{
            for (int i = 0; i < image.rows; ++i){
                T* pCvRow = image.ptr<T>(i);
                for (int j = 0; j < image.cols; ++j){
                    *(pCvRow + j) = *((T*)(pImg->pImageData[0] + i * pImg->strides[0]) + j);
                }
            }
            break;
        }
        case Y_C_C_D_RGGB:
        case Y_C_C_D_GRBG:
        case Y_C_C_D_GBRG:
        case Y_C_C_D_BGGR:{
            for (int i = 0; i < image.rows; ++i){
                T* pCvRow = image.ptr<T>(i);
                for (int j = 0; j < image.cols; ++j){
                    *(pCvRow + j) = *((T*)(pImg->pImageData[panelId] + i * pImg->strides[panelId]) + j);
                }
            }
            break;
        }
        default:{
            std::cout<<"error: image format is not supported. exited.\n";
            exit(1);
        }
    }
}

typedef void (*FCOPY2)(cv::Mat&, const Img_t*, const int);

void convert_img_t_to_cv_mat(cv::Mat& image, const Img_t* pImg, const int panelId){
    //  cv::Mat.type()
    //          C1 	C2 	C3 	C4 
    // CV_8U   0   8   16	24
    // CV_8S   1   9	17	25
    // CV_16U  2   10	18	26
    // CV_16S  3   11	19	27
    // CV_32S  4   12	20	28
    // CV_32F  5   13	21	29
    // CV_64F  6   14	22	30 

    int type = 0;
    int typeOffset = 0;
    switch (pImg->imageFormat){
        case Y_C_C_D_RGGB:
        case Y_C_C_D_GRBG:
        case Y_C_C_D_GBRG:
        case Y_C_C_D_BGGR:
        case RAW_RGGB:
        case RAW_GRBG:
        case RAW_GBRG:
        case RAW_BGGR:
        case MONO:{
            typeOffset = 0;
            break;
        }
        case RGB:{
            typeOffset = 16;
            break;
        }
        default:{
            std::cout<<"error: image format is not supported. exited.\n";
            exit(1);
        }
    }
    FCOPY2 f = NULL;
    if (pImg->sign == UNSIGNED){
        if (pImg->bitDepth <= 8){
            type = 0 + typeOffset;
            f = copy_data_from_img_t_to_cv_mat<uint8_t>;
        }
        else if (pImg->bitDepth <= 16){
            type = 2 + typeOffset;
            f = copy_data_from_img_t_to_cv_mat<uint16_t>;
        }
        else if (pImg->bitDepth <= 32){
            std::cout<< "error: cv mat does not support uint32_t. exited.\n";
            exit(1);
        }
    }
    else{
        if (pImg->bitDepth <= 8){
            type = 1 + typeOffset;
            f = copy_data_from_img_t_to_cv_mat<int8_t>;
        }
        else if (pImg->bitDepth <= 16){
            type = 3 + typeOffset;
            f = copy_data_from_img_t_to_cv_mat<int16_t>;
        }
        else if (pImg->bitDepth <= 32){
            type = 4 + typeOffset;
            f = copy_data_from_img_t_to_cv_mat<int32_t>;
        }
    }
    image.create(pImg->height, pImg->width, type);
    assert(f != NULL);
    f(image, pImg, panelId); // panelId is not always necessary, but when using a function pointer, it should be specified
}

void convert_img_t_to_cv_mat(cv::Mat* pCvImageArray, const Img_t* pImg){ // overloaded
    for(int i = 0; i < MAX_NUM_P; ++i){
        if (pImg->pImageData[i] != NULL){
            convert_img_t_to_cv_mat(*(pCvImageArray+i), pImg, i); // original
        }
    }
}


void test_opencv(){
    //Mat image(15, 15, CV_8UC3, Scalar(3,9,5));
    //Mat image(15, 15, CV_32SC1, Scalar(-8));

    Mat image;
    image = imread( "../data/anya18.png", IMREAD_COLOR );
    if ( !image.data ){
        std::cout<<"No image data \n";
    }
    // namedWindow("Display Image", WINDOW_AUTOSIZE );
    // imshow("Display Image", image);
    // waitKey(0);


    std::cout<<" channels:" << image.channels() <<"\n";
    std::cout<<" step:" << image.step[0] <<"\n";
    std::cout<<" step:" << image.step[1] <<"\n";
    std::cout<<" step:" << image.step[2] <<"\n";
    std::cout<<" size:" << image.size <<"\n";
    std::cout<<" row, col:" << image.rows <<" "<<image.cols <<"\n";
    std::cout<<" type:" << image.type() <<"\n";


    Img_t* pMyImg = NULL; // initialize
    pMyImg =(Img_t*)malloc(sizeof(Img_t));

    convert_cv_mat_to_img_t(image, pMyImg, 32, true, SIGNED, 16);
    
    ROI_t viewROI = {0, 0, 0, 15, 15};
    view_image_data(pMyImg, viewROI );
    view_img_properties(pMyImg);

    Mat image2;
    convert_img_t_to_cv_mat(image2, pMyImg);
    for (int i = 0; i < 15; ++i){
        for (int j = 0; j < 15*3; ++j){
            std::cout<<" "<<image2.at<int16_t>(i,j);
        }
        std::cout<<"\n";
    }
    // std::cout<<" channels:" << image2.channels() <<"\n";
    // std::cout<<" step:" << image2.step[0] <<"\n";
    // std::cout<<" step:" << image2.step[1] <<"\n";
    // std::cout<<" step:" << image2.step[2] <<"\n";
    // std::cout<<" size:" << image2.size <<"\n";
    // std::cout<<" row, col:" << image2.rows <<" "<<image2.cols <<"\n";
    // std::cout<<" type:" << image2.type() <<"\n";

    imwrite("../dump/out.png", image2);
    
    destruct_img(&pMyImg);
}



void test_img_t_to_multiple_cv_mat(){
    Img_t* pImg =(Img_t*)malloc(sizeof(Img_t));
    IMAGE_FMT imageFormat = Y_C_C_D_RGGB;
    int width = 6;
    int height = 6;
    SIGN sign = SIGNED;
    int bitDepth = 16;
    int alignment = 32;
    bool allocateImage = true; 

    construct_img(pImg, 
                  imageFormat,
                  width,
                  height,
                  sign,
                  bitDepth,
                  alignment,
                  allocateImage); 

    ValCfg_t sValCfg = {pImg->sign, rand_num_uniform, {0, 49, 0, 0}};

    set_value(pImg, sValCfg);

    
    std::cout<<"original:\n";
    for (int i = 0; i < 4; ++i){
        ROI_t viewROI = {i, 0, 0, pImg->width, pImg->height};
        view_image_data(pImg, viewROI);
        std::cout<<"\n";
    }

    Mat pCvImageArray[4];
    convert_img_t_to_cv_mat(pCvImageArray, pImg);

    std::cout<<"convert to cv mat:\n";
    for (int a = 0; a < 4; ++a){
        for (int i = 0; i < pImg->height; ++i){
            for (int j = 0; j < pImg->width; ++j){
                std::cout<<" "<<pCvImageArray[a].at<int16_t>(i,j);
            }
            std::cout<<"\n";
        }
        std::cout<<"\n";
    }

}



// test read_raw_to_img_t() in "ImageIO.hpp";
// it needs opencv to dump image, so it is moved here.
void test_read_raw(){
    Img_t* pImg = NULL;
    pImg =(Img_t*)malloc(sizeof(Img_t));
    IMAGE_FMT imageFormat = RAW_RGGB;
    int width = 4256;
    int height = 2848;
    int bitDepth = 16;
    int alignment = 1;
    
    read_raw_to_img_t("../data/rawData.raw",
                      pImg,
                      imageFormat,
                      width,
                      height,
                      bitDepth,
                      alignment);

    view_img_properties(pImg);

    Mat image;
    convert_img_t_to_cv_mat(image, pImg);
    imwrite("../dump/raw_preview.png", image);
    
    Mat image2;
    image2 = imread( "../dump/raw_preview.png", IMREAD_UNCHANGED);
    if ( !image2.data ){
        std::cout<<"No image data \n";
    }

    Img_t* pImg2 = NULL;
    pImg2 =(Img_t*)malloc(sizeof(Img_t));
    convert_cv_mat_to_img_t(image2, pImg2, alignment, false);


    std::cout<<"is equal: "<<is_image_equal(pImg, pImg2)<<"\n";
    destruct_img(&pImg2);
    destruct_img(&pImg);

}

