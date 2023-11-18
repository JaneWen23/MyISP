#include <stdio.h>
#include "ImageIO.hpp"
#include <opencv2/opencv.hpp>
using namespace cv;
void test_opencv(){
    Mat image;
    
    image = imread( "anya18.png", IMREAD_COLOR );
    if ( !image.data )
    {
        printf("No image data \n");
    }
    namedWindow("Display Image", WINDOW_AUTOSIZE );
    imshow("Display Image", image);
    waitKey(0);
}