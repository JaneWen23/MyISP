#include <iostream>
#include "src/Infra/ImgDef.hpp"
#include "src/Math/Math.hpp"
#include "src/Infra/RandImageGen.hpp"
#include "src/DWT/dwt.hpp"
#include "src/Filter/SlidingWindow.hpp"
#include "src/Infra/ImageIO.hpp"
#include "src/StarTetrix/StarTetrixTrans.hpp"
#include "src/Demo/demo.hpp"
#include "src/CSC/Color.hpp"
#include <functional>


std::function<IMG_RTN_CODE(Img_t*, Img_t*, void*)> callback;



int main(){
    //test_opencv();
    //test_dwt();
    //demo_dwt();
    //test_star_tetrix();
    //test_read_raw();
    //test_img_t_to_multiple_cv_mat();
    //demo_star_tetrix();
    //test_ccm();
    //rgb_to_yuv420_prototype();
    callback = ccm;
    //std::cout<< callback(pInImg, pOutImg, pCCMArg)<<"\n";
    std::cout<<"hello world!!\n";
    std::cout<<"hello Cpp!!\n";
    return 0;
}