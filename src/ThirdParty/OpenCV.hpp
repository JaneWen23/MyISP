#ifndef _OPEN_CV_H
#define _OPEN_CV_H

#include <opencv2/opencv.hpp>
#include "../Algos/Infra/ImgDef.hpp"

void convert_cv_mat_to_img_t(cv::Mat& image, Img_t* pImg, const int alignment, bool needOverride = false, SIGN dstSign = UNSIGNED, int dstBitDepth = 0);
void convert_img_t_to_cv_mat(cv::Mat& image, const Img_t* pImg, const int panelId = 0);
void convert_img_t_to_cv_mat(cv::Mat* pCvImageArray, const Img_t* pImg);

void test_opencv();
void test_img_t_to_multiple_cv_mat();
void test_read_raw();

#endif