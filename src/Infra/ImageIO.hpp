#ifndef _IMAGE_IO_H
#define _IMAGE_IO_H
#include "ImgDef.hpp"
#include <opencv2/opencv.hpp>

void convert_cv_mat_to_img_t(cv::Mat& image, Img_t* pImg, const int alignment, bool needOverride = false, SIGN dstSign = UNSIGNED, int dstBitDepth = 0);
void convert_img_t_to_cv_mat(cv::Mat& image, const Img_t* pImg, const int panelId = 0);
void convert_img_t_to_cv_mat(cv::Mat* pCvImageArray, const Img_t* pImg);
void read_raw_to_img_t(const char* path,
                       Img_t* pImg,
                       const IMAGE_FMT imageFormat,
                       const int width,
                       const int height,
                       const int bitDepth,
                       const int alignment);

void test_opencv();
void test_read_raw();
void test_img_t_to_multiple_cv_mat();
#endif