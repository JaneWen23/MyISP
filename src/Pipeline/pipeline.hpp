#ifndef _PIPELINE_H
#define _PIPELINE_H

#include <iostream>
#include <list>
#include "../Algos/Infra/ImgDef.hpp"
#include "../Modules/COMMON/PipelineModules.hpp"


typedef struct{
    // the integration of all run-time arguments
    ReadRawArg_t sVinArg;
    MArg_Compression_t sCompressionArg;
    CCMArg_t sCCMArg;
} PipeArgs_t;

class Pipeline{
    public:
        Pipeline(Img_t& sImg);
        ~Pipeline();
        void add_module_to_pipe(Module_t& sModule);
        void print_pipe();
        // TODO: add func to check in_fmt, out_fmt, in_bitDepth, out_bitDepth
        void run_pipe(PipeArgs_t& sArgs); // run pipeline for a single frame
        void dump();

    private:
        bool is_pipe_valid_till_now(Module_t& sModule);
        void set_in_img_t(Module_t& sModule); // set _sInImg according to current running sModule
        void move_data();

    private:
        ImgPtrs_t _sInImgPtrs;
        Img_t _sOutImg;
        // TODO: const char* namePrefix ???

    protected:
        std::list<Module_t> _pipe;
};

class StreamPipeline : public Pipeline{
    public:
        StreamPipeline(Img_t& sImg); // maybe add cfg to parameter list
        ~StreamPipeline();
        void frames_run_pipe(PipeArgs_t& sArgs); // TODO: maybe another name??
    private:
        void update_module_args(int frameInd);//TODO: to be finished, update args every frame.
    private:
        int _startFrameInd;
        int _frameNum;
        // const char* namePrefix_2 ???
};

void test_pipeline();

#endif