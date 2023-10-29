
#include "Conv.hpp"

// sliding window: a uniform API to perform 1D/2D convolution of any kernel, with step configurable.
// SlidingWindow(ImgIn, ROI, ImgOut, KernelCfg), and ROI addr_next_sart = offset(starting byte) + bytes_per_line

// ROI: original image starting row and column, and ROI length and width
// KernelCfg: pkernel, padding, step
// 1d kernel pointer is just fine; 2d kernel pointer, store as row-by-row?

// 1D (or separable) convolution: 
 
// okay but is it necessary to separate, if we are doing "line buffer" processing?

void test_conv(){
    
}
