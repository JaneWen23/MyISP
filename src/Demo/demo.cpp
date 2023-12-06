#include "demo.hpp"
#include "../Infra/ImageIO.hpp"
#include "../StarTetrix/StarTetrixTrans.hpp"
#include "../DWT/dwt.hpp"

using namespace cv;

void demo_star_tetrix(){
    Img_t* pInImg = NULL;
    pInImg =(Img_t*)malloc(sizeof(Img_t));
    IMAGE_FMT imageFormat = RAW_RGGB;
    int width = 4256; // 4240
    int height = 2848; // 2832
    int bitDepth = 16;
    int alignment = 1; //32
    
    read_raw_to_img_t("../data/rawData.raw", // current working directory is build/.
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
    
    imwrite("../dump/star_tetrix_Ybar.png", pCvImageArray[0]-512); // -512 to look good
    imwrite("../dump/star_tetrix_Cb.png", pCvImageArray[1]+256); // +256 to look good
    imwrite("../dump/star_tetrix_Cr.png", pCvImageArray[2]+256); // +256 to look good
    imwrite("../dump/star_tetrix_Delta.png", pCvImageArray[3]);

    Img_t* pBackOutImg =(Img_t*)malloc(sizeof(Img_t));
    star_tetrix_backward(pOutImg, pBackOutImg, (void*)pStarTetrixArg);

    std::cout<<"is equal: "<<is_image_equal(pInImg, pBackOutImg)<<"\n";

    pBackOutImg->sign = UNSIGNED; // to make cv mat happy
    Mat image;
    convert_img_t_to_cv_mat(image, pBackOutImg);
    imwrite("../dump/star_tetrix_backward.png", image); // current working directory is build/.
}


void demo_dwt(){
    Mat image;
    image = imread( "../data/anya18.png", IMREAD_GRAYSCALE ); // current working directory is build/.
    if ( !image.data )
    {
        std::cout<<"No image data \n";
    }
    Img_t* pImg =(Img_t*)malloc(sizeof(Img_t));
    convert_cv_mat_to_img_t(image, pImg, 32, true, SIGNED, 16);

    DWTArg_t* pDWTArg = (DWTArg_t*)malloc(sizeof(DWTArg_t));
    pDWTArg->level = 1;
    pDWTArg->orient = TWO_DIMENSIONAL;
    pDWTArg->inImgPanelId = 0;
    pDWTArg->outImgPanelId = 0;
    config_dwt_kernels_LeGall53<int16_t>(pDWTArg, MIRROR);

    Img_t* pOutImg = (Img_t*)malloc(sizeof(Img_t));

    dwt_forward(pImg, pOutImg, (void*)pDWTArg);

    // ROI_t viewROI = {0, 600, 600, 20, 20};
    // view_image_data(pImg, viewROI );

    Mat image2;
    convert_img_t_to_cv_mat(image2, pOutImg);
    imwrite("../dump/dwtout.png", image2); // current working directory is build/.

    Img_t* pOutBackImg = (Img_t*)malloc(sizeof(Img_t));
    dwt_backward(pOutImg, pOutBackImg, (void*)pDWTArg);
    
    Mat image3;
    convert_img_t_to_cv_mat(image3, pOutBackImg);
    imwrite("../dump/idwtout.png", image3); // current working directory is build/.

    destruct_img(&pImg);
    destruct_img(&pOutImg);
    destruct_img(&pOutBackImg);
}