#ifndef _PIPELINE_H
#define _PIPELINE_H

#include <list>
#include "../Algos/Infra/ImgDef.hpp"
#include "../Modules/COMMON/PipelineModules.hpp" // TODO: just want an integral of all modules. can be anoher file or file name.
#include "graph.hpp"


typedef struct{
    // the integration of all module arguments (beginning with "MArg")
    MArg_Vin_t sVinArg;
    MArg_Compression_t sCompressionArg;
    MArg_CCM_t sCCMArg;
} PipeArgs_t;

typedef struct{
    MODULE_NAME module;
    std::vector<MODULE_NAME> pred_modules; // predecessor modules; a module is a "name" of a vertex
    std::vector<MODULE_NAME> succ_modules; // successor modules; a module is a "name" of a vertex
    std::function<IMG_RTN_CODE(const ImgPtrs_t, Img_t*, void*)> run_function;
} Module_t; // "full", and is inferred from graph and modules

typedef std::vector<Module_t> Pipe_t;

void print_pipe(Pipe_t& pipe);


class Pipeline{
    public:
        Pipeline(const Graph_t& graph, bool needPrint); // TODO: module cfg will be carried out in pipeline class constructor
        ~Pipeline();
        //void add_module_to_pipe(Module_t& sModule);
        // TODO: add func to check in_fmt, out_fmt, in_bitDepth, out_bitDepth????
        // TODO: run-time log of in_fmt, out_fmt, in_bitDepth, out_bitDepth????
        void run_pipe(PipeArgs_t& sArgs); // run pipeline for a single frame
        void dump();

    private:
        bool is_pipe_valid_till_now(Module_t& sModule); // TODO: remove?
        void set_in_img_t(Module_t& sModule); // set _sInImg according to current running sModule
        void move_data();

    private:
        ImgPtrs_t _sInImgPtrs; // TODO: input pool + "madeFrom, deliverTo"
        Img_t _sOutImg; // TODO: when made: record madeFrom, deliverTo
        // TODO: const char* namePrefix ???

    protected:
        MODULE_NAME* _sorted;
        Pipe_t _pipe;
};

class StreamPipeline : public Pipeline{
    public:
        StreamPipeline(const Graph_t& graph, bool needPrint); // maybe add cfg to parameter list
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