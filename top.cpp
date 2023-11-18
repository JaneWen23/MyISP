#include <iostream>
#include "src/Infra/ImgDef.hpp"
#include "src/Math/Math.hpp"
#include "src/Infra/RandImageGen.hpp"
#include "src/DWT/dwt.hpp"
#include "src/Filter/SlidingWindow.hpp"

#include <opencv2/opencv.hpp>
using namespace cv;

using namespace std;

int main(){
    Mat image;
    image = imread( "anya18.png", IMREAD_COLOR );
    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }
    namedWindow("Display Image", WINDOW_AUTOSIZE );
    imshow("Display Image", image);
     waitKey(0);


    //test_dwt();
    std::cout<<"hello world!!\n";
    std::cout<<"hello Cpp!!\n";
    return 0;
}