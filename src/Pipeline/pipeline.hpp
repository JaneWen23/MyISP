#ifndef _PIPELINE_H
#define _PIPELINE_H

#include <iostream>
#include <list>
#include <functional>
#include "../Modules/Infra/ImgDef.hpp"


typedef enum {
    ISP_VIN,
    ISP_BLC,
    ISP_DMS,
    ISP_WB,
    ISP_CCM,
    ISP_RGB2YUV
} MODULE_ENUM;

typedef struct{
    MODULE_ENUM moduleEnum;
    IMAGE_FMT inFmt;
    IMAGE_FMT outFmt;
    int inBitDepth;
    int outBitDepth;
    SIGN inSign;
    SIGN outSign;
    void* pArg;
    std::function<IMG_RTN_CODE(Img_t*, Img_t*, void*)> run_function;
} Module_t;

// store all arguments to all modules
// if a module actually includes two modules, what should be done?

class Pipeline{
    public:
        Pipeline(int startFrameInd, int frameNum);
        ~Pipeline();
        void add_module_to_pipe(Module_t& sModule);
        void print_pipe();
        void init_pipe();
        void set_img_t(); // set _sInImg according to current running sModule and the private members
        void run_pipe();

    private:
        int _width;
        int _height;
        int _alignment;
        Img_t _sInImg;
        Img_t _sOutImg;
        int _startFrameInd;
        int _frameNum;
        int _currentFrameInd;
        std::list<Module_t> _pipe;
};

void test_pipeline();

#endif