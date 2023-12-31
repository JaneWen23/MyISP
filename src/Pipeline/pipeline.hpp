#ifndef _PIPELINE_H
#define _PIPELINE_H

#include <list>
#include "../Modules/COMMON/PipelineModules.hpp" // TODO: just want an integral of all modules. can be anoher file or file name.
#include "graph.hpp"

typedef struct{
    MODULE_NAME madeBy;
    std::vector<MODULE_NAME> deliverTo;
} Signature_t;

typedef struct{
    MODULE_NAME module;
    std::vector<MODULE_NAME> predInOrder;
} InputOrder_t; // is needed only when module is with two or more inputs

typedef std::vector<InputOrder_t> Orders_t; // only stores the info of modules with two or more inputs; TOML;

typedef struct{
    Img_t img;
    Signature_t sig;
} PipeImg_t;

typedef struct{
    MODULE_NAME module;
    std::vector<MODULE_NAME> pred_modules; // predecessor modules; a module is a "name" of a vertex
    std::vector<MODULE_NAME> succ_modules; // successor modules; a module is a "name" of a vertex
    std::function<IMG_RTN_CODE(const ImgPtrs_t, Img_t*, void*)> run_function;
    // more: graph in "time"???
} Module_t; // "full", and is inferred from graph and modules

typedef std::vector<Module_t> Pipe_t;

void print_pipe(const Pipe_t& pipe);
void parse_args(const int frameInd, AllArgs_t& sArgs);//TODO: to be finished, update args every frame.


class Pipeline{
    public:
    Pipeline(const Graph_t& graph, const Orders_t& orders, bool needPrint);
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

    protected:
        MODULE_NAME* _sorted;
        Pipe_t _pipe;
        PipeImg_t _sOutPipeImg;

    private:
        std::vector<PipeImg_t> _InImgPool; 
        // TODO: const char* namePrefix ???

};

class StreamPipeline : public Pipeline{
    public:
        StreamPipeline(const Graph_t& graph, const Orders_t& orders, bool needPrint);
        ~StreamPipeline();
        void frames_run_pipe(AllArgs_t& sArgs); // TODO: maybe another name?? 
        void run_pipe(AllArgs_t& sArgs);

    private:
        void run_module(const Module_t& sModule, void* pMArg);
        // graph of "time", the edges between frames
        int _startFrameInd;
        int _frameNum;
        // const char* namePrefix_2 ???
};

void test_pipeline();

#endif