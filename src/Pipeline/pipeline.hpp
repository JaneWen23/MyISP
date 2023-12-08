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
        Pipeline(Img_t& sImg);
        ~Pipeline();
        void add_module_to_pipe(Module_t& sModule);
        void print_pipe();
        void run_pipe(); // for each frame, run the pipe
        void dump();
       //void frame_run_pipe();
        //void loop_run_pipe(); // TODO: maybe "loop" is a different object??? only thing need take care of is the dumped file name

    private:
        bool is_pipe_valid_till_now(Module_t& sModule);
        void set_in_img_t(Module_t& sModule); // set _sInImg according to current running sModule
        void move_data();

    private:
        Img_t _sInImg;
        Img_t _sOutImg;
        // int _startFrameInd; // remove?
        // int _frameNum;
        // int _currentFrameInd; // make local variable?
        std::list<Module_t> _pipe;
        // loop start, total loops, current loop
};

void test_pipeline();

#endif