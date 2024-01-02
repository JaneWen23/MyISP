#ifndef _PIPELINE_H
#define _PIPELINE_H

#include <list>
#include "../Modules/COMMON/PipelineModules.hpp" // TODO: just want an integral of all modules. can be anoher file or file name.
#include "graph.hpp"

typedef struct ModuleDelay_tag{
    MODULE_NAME module;
    int delay = 0;
} ModuleDelay_t;

typedef struct{
    MODULE_NAME module;
    int time;
} ModuleTime_t;

typedef struct{
    ModuleTime_t madeBy;
    std::vector<ModuleDelay_t> deliverTo;
} Signature_t;

typedef struct{
    Img_t img;
    Signature_t sig;
} PipeImg_t;

typedef struct{
    MODULE_NAME module;
    std::vector<ModuleDelay_t> inputInOrder;
} InputOrder_t; // is needed only when module is with two or more inputs
// TODO: for no-delay pipes, inputInOrder from cfg may not contain delay info. add mechanism to convert no delay to delayed.

typedef std::vector<InputOrder_t> Orders_t; // only stores the info of modules with two or more inputs; TOML;


typedef struct{
    MODULE_NAME module;
    std::vector<ModuleDelay_t> succWthDelay;
} DelayAdjacency_t;

typedef std::vector<DelayAdjacency_t> DelayGraph_t;


typedef struct Module_tag{
    MODULE_NAME module;
    std::vector<ModuleDelay_t> predWthDelay; // predecessor modules with indication of delay; a module is a "name" of a vertex
    std::vector<ModuleDelay_t> succWthDelay; // successor modules with indication of delay; a module is a "name" of a vertex 
    std::function<IMG_RTN_CODE(const ImgPtrs_t, Img_t*, void*)> run_function;
} Module_t; // "full", and is inferred from graphs and modules

typedef std::vector<Module_t> Pipe_t;

void print_pipe(const Pipe_t& pipe);
void parse_args(const int frameInd, AllArgs_t& sArgs);


class Pipeline{
    public:
    Pipeline(const Graph_t& graphNoDelay, const DelayGraph_t delayGraph, const Orders_t& orders, bool needPrint);
        ~Pipeline();
        // TODO: add func to check in_fmt, out_fmt, in_bitDepth, out_bitDepth????
        // TODO: run-time log of in_fmt, out_fmt, in_bitDepth, out_bitDepth????
        virtual void run_pipe(AllArgs_t& sArgs); // run pipeline for a single frame
        void dump();

    protected:
        void move_output_to_pool();
        const ImgPtrs_t distribute_in_img_t(const Module_t& sModule); // distribute who is main img, who is additional img, ...
        void sign_out_from_pool(const Module_t& sModule); // after input img is used, remove module name from deliverTo list; if list is empty, destroy the img.
        void signature_output_img(const Module_t& sModule); // assemble the img and signature
        void run_module(const Module_t& sModule, void* pMArg);
        void clear_imgs();

    protected:
        MODULE_NAME* _sorted;
        Pipe_t _pipe;
        PipeImg_t _sOutPipeImg;
        int _frameInd = 0;

    private:
        std::vector<PipeImg_t> _InImgPool; // SHARED by Pipeline and StreamPipeline!!!
        // TODO: add offline mode, load some offline img
        // TODO: const char* namePrefix ???

};

class StreamPipeline : public Pipeline{
    public:
        StreamPipeline(const Graph_t& graphNoDelay, const DelayGraph_t delayGraph, const Orders_t& orders, bool needPrint);
        ~StreamPipeline();
        void frames_run_pipe(AllArgs_t& sArgs, int startFrameInd, int frameNum); // TODO: maybe another name?? 
        void run_pipe(AllArgs_t& sArgs);

    private:
        void run_module(const Module_t& sModule, void* pMArg);
        // int _startFrameInd;
        // int _frameNum;
        // const char* namePrefix_2 ???
};

void test_pipeline();

#endif