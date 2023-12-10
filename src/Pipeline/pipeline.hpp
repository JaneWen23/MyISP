#ifndef _PIPELINE_H
#define _PIPELINE_H

#include <iostream>
#include <list>
#include "../Modules/Infra/ImgDef.hpp"
#include "../Modules/PipelineModules/PipelineModules.hpp"

typedef struct{
    // the integration of all run-time arguments
    ReadRawArg_t sVinArg;
    MyJXSArg_t sCompressionArg;
    CCMArg_t sCCMArg;
} Args_t;

class Pipeline{
    public:
        Pipeline(Img_t& sImg);
        ~Pipeline();
        void add_module_to_pipe(Module_t& sModule);
        void print_pipe();
        void run_pipe(Args_t& sArgs); // run pipeline for a single frame
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
        void frames_run_pipe(Args_t& sArgs); // TODO: maybe another name??
    private:
        void update_module_args(int frameInd);//TODO: move out of class.
    private:
        int _startFrameInd;
        int _frameNum;
        // const char* namePrefix_2 ???
};

void test_pipeline();

#endif