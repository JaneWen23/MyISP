#include <iostream>
#include "src/Algos/Infra/ImgDef.hpp"
#include "src/Algos/Math/Math.hpp"
#include "src/Algos/Infra/RandImageGen.hpp"
#include "src/Algos/DWT/dwt.hpp"
#include "src/Algos/Filter/SlidingWindow.hpp"
#include "src/Algos/Infra/ImageIO.hpp"
#include "src/Algos/StarTetrix/StarTetrixTrans.hpp"
#include "src/Modules/ISP_COMPRESSION/MyJXS.hpp"
#include "src/Demo/demo.hpp"
#include "src/Algos/Color/Color.hpp"
#include "src/Pipeline/pipeline.hpp"
#include <functional>



int main(){
    //test_opencv();
    //test_dwt();
    //demo_dwt();
    //test_star_tetrix();
    //test_read_raw();
    //test_img_t_to_multiple_cv_mat();
    //demo_star_tetrix();
    //test_ccm(); 
    //test_pipeline();
    //test_my_jxs();
    std::cout<< INT32_MAX <<"\n";
    std::cout<< UINT32_MAX <<"\n";
    std::cout<< INT16_MAX <<"\n";
    std::cout<< UINT16_MAX <<"\n";
    std::cout<<"hello world!!\n";
    std::cout<<"hello Cpp!!\n";
    return 0;
}