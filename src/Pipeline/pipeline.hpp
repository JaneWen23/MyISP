#ifndef _PIPELINE_H
#define _PIPELINE_H

#include <iostream>
#include <list>
#include <functional>
#include "../Modules/Infra/ImgDef.hpp"
#include "../Modules/Infra/ImageIO.hpp"


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

typedef struct{
    ReadRawArg_t sReadRawArg;

} Arg_t;

typedef struct{
    // startFrameInd, frameNum
    // module name list in order
    // 
    // new pArg's for every frame, or necessary frames
} PipeCfg_t;

class Pipeline{
    public:
        Pipeline(Img_t& sImg);
        ~Pipeline();
        void add_module_to_pipe(Module_t& sModule);
        void print_pipe();
        void run_pipe(); // run pipeline for a single frame
        void dump();

    private:
        bool is_pipe_valid_till_now(Module_t& sModule);
        void set_in_img_t(Module_t& sModule); // set _sInImg according to current running sModule
        void move_data();

    private:
        Img_t _sInImg;
        Img_t _sOutImg;

    protected:
        std::list<Module_t> _pipe;
};

class StreamPipeline : public Pipeline{
    public:
        StreamPipeline(Img_t& sImg); // maybe add cfg to parameter list
        ~StreamPipeline();
        void frames_run_pipe(); // maybe another name??
    private:
        void update_module_args(int frameInd);//should input cfg
    private:
        int _startFrameInd;
        int _frameNum;
};

void test_pipeline();

#endif