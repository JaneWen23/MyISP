#include <iostream>
#include "src/Infra/ImgDef.hpp"
#include "src/Math/Math.hpp"
#include "src/Infra/RandImageGen.hpp"
#include "src/DWT/dwt.hpp"
#include "src/Filter/SlidingWindow.hpp"
#include "src/Infra/ImageIO.hpp"
#include "src/StarTetrix/StarTetrixTrans.hpp"

using namespace std;

int main(){
    //test_opencv();
    //demo_dwt();
    //test_star_tetrix();
    //test_read_raw();
    //test_img_t_to_multiple_cv_mat();
    //demo_star_tetrix();
    char* p = (char*)malloc(10);
    free(p);
    p[0] = 1;
    std::cout<<"hello world!!\n";
    std::cout<<"hello Cpp!!\n";
    return 0;
}