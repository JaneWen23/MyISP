#ifndef _PIPELINE_H
#define _PIPELINE_H

#include <iostream>
#include <list>
#include "../Modules/PipelineModules/PipelineModules.hpp"


typedef struct{
    // startFrameInd, frameNum, rewind
    // module name list in order
    // which modules will change args? how to update args??
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
        // const char* namePrefix ???

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
        // const char* namePrefix_2 ???
};

void test_pipeline();

#endif