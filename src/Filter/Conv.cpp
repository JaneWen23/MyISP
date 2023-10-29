
#include "Conv.hpp"

// sliding window: a uniform API to perform 1D/2D convolution of any kernel, with tep configurable.
//TODO: SlidingWindow(ImgIn, ROI, ImgOut, KernelCfg), and ROI addr_next_sart = offset(starting byte) + bytes_per_line

// ROI: original image starting row and column, and ROI length and width
// KernelCfg: kernel, padding, step


// 1D (or separable) convolution: 
 
// okay but is it necessary to separate, if we are doing "line buffer" processing?

void test_conv(){
    
}
