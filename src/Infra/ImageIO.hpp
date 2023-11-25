#ifndef _IMAGE_IO_H
#define _IMAGE_IO_H
#include "ImgDef.hpp"
#include <opencv2/opencv.hpp>

void convert_cv_mat_to_img_t(cv::Mat& image, Img_t* pImg, const int alignment, bool needOverride = false, SIGN dstSign = UNSIGNED, int dstBitDepth = 0);
void convert_img_t_to_cv_mat(cv::Mat& image, Img_t* pImg);

void test_opencv();
void test_read_raw();
#endif