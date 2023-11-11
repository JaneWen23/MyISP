# Sliding Window Introduction

sliding_window() is a function to implement 1D and 2D image filtering with a finite kernel, or window. The kernel can be a convolution kernel or can be any user-defined kernel, either explicit or implicit.

## 1. Features

### 1.1. Convolution

√ int-to-int convolution

√ 1D / 2D convolution with padding scheme

√ 1D / 2D horizontal / vertical stride(aka. step) of kernel sliding

√ 1D / 2D horizontal / vertical upsample after filtering

√ option to flip the kernel or not

### 1.2. Self-Defined Filtering

√ padding schemes, stride(aka. step), upsample, flip kernel or not

√ user provide the formula that defines the filter in the single window, then sliding_window() will perform the filtering

√ user can provide an explicit 1D / 2D kernel with a self-defined formula

√ also supports the case when no explicit kernel needed

√ "divide and conquer" the implementation of 2D filtering if the formula has associativity

## 2. Input and Output

### 2.1. Input Arguments (what are they and how to set them)

    const Img_t* pInImg,
    const ROI_t& sInImgROI,
    Img_t* pOutImg, 
    const ROI_t& sOutImgROI, 
    const KernelCfg_t& sKernelCfg

The input / output image and the ROI structures are defined in "ImgDef.cpp" and they are easy to understand. (The output image ROI means to )

Up to 32-bit bit-depth is supported, regardless of signed or unsigned and input / output image should be integers (currently it does not support float types). 

Also note that although the output image may be signed while the input image being unsigned, the input and output images should be set to the same bit-depth. To prevent a large unsigned number to be interpreted as negative, you may choose a higher bit-depth for both.

The KernelCfg structure defines the kernel itself, filter behaviors, and the option related to 2D implementation:

* members about the kernel itself:

    *    uint8_t* pKernel; // pointer to kernel data; may convert to any type when used; can be NULL if no explicit kernel.
    *    int kerHeight; // if no explicit kernel, this is equivalently window height, minimum is 1
    *    int kerWidth; // if no explicit kernel, this is equivalently window width, minimum is 1
    *    int horiCenter; // horizontal center (if explicit kernel)
    *    int vertCenter; // vertical center (if explicit kernel)

* members about the filtering behaviors:

    *   PADDING padding; // padding scheme, including zero-padding, periodic padding and mirror padding.
    *   int horiStep; // horizontal step (same effect as filtering followed by down-sampling horizontally)
    *   int vertStep; // vertical step (same effect as filtering followed by down-sampling vertically)
    *   int horiUpsample; // (down-sampled (or not)) filtering followed by up-sampling horizontally
    *   int vertUpsample; // (down-sampled (or nor)) filtering followed by up-sampling vertically
    *   bool needFlip; // if true, it is convolution; if false, it is correlation.
    *   void* formula; // function pointer. use dot_product for convolution; self-defined formula for other filtering.
  
* member related to the way the program carrying out 2D filtering:

    *   bool divideAndConquer; // if true, 2d filtering can be defined by summation of several 1d results; if false, 2d filtering will be performed in naive way. it must be true if you are doing convolution.

Note that only integers are allowed for the explicit kernel. The fraction kernels may be defined implicitly, but the filter result should also be rounded to integers.

Although the kernel does not specify bit-depth, when calling sliding_window(), the kernel bit-depth will be treated always as the same as the output image. The user should take care of the actual kernel values.

### 2.2. Output

The output of sliding_window() is just a status, which is not of interest currently.

The output image, which is treated as an input argument, has several things to be taken care of:

* the output image must be allocated before passed to sliding_window(), because the program won't create one but writes to the location of output image directly.

* the sign and the bit-depth: output image may be signed, but should have the same bit-depth as the input image. So choose a proper bit-depth for both input and output images.

* output image does not need to have the same size and/or panels as input image.
  
* horizontal / vertical step, horizontal / vertical upsample: they together decide the output image width and height. for example, if step = 2 and upsample = 1 (1 means no upsample), the output size should be half of the input; if step = 2 and upsample = 2, the output size equals to the input.


