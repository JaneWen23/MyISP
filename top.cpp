#include <iostream>
#include "src/Modules/Infra/ImgDef.hpp"
#include "src/Modules/Math/Math.hpp"
#include "src/Modules/Infra/RandImageGen.hpp"
#include "src/Modules/DWT/dwt.hpp"
#include "src/Modules/Filter/SlidingWindow.hpp"
#include "src/Modules/Infra/ImageIO.hpp"
#include "src/Modules/StarTetrix/StarTetrixTrans.hpp"
#include "src/Modules/Compression/MyJXS.hpp"
#include "src/Demo/demo.hpp"
#include "src/Modules/CSC/Color.hpp"
#include "src/Pipeline/pipeline.hpp"
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
    //callback = ccm;
    //std::cout<< callback(pInImg, pOutImg, pCCMArg)<<"\n";
    //test_pipeline();
    test_my_jxs();
    std::cout<<"hello world!!\n";
    std::cout<<"hello Cpp!!\n";
    return 0;
}