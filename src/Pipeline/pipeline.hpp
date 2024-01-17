#ifndef _PIPELINE_H
#define _PIPELINE_H

#include "../Modules/COMMON/PipelineModules.hpp" // TODO: just want an integral of all modules. can be anoher file or file name.
#include "graph.hpp"

typedef struct ModuleDelay_tag{
    MODULE_NAME module;
    int delay = 0;
} ModuleDelay_t;

typedef struct{
    MODULE_NAME module;
    int timeStamp;
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
    std::function<IMG_RTN_CODE(const ImgPtrs_t, Img_t*, Hash_t*)> run_function;
} Module_t; // "full", and is inferred from graphs and modules

typedef std::vector<Module_t> Pipe_t;

void print_pipe(const Pipe_t& pipe);


class Pipeline{
    public:
        Pipeline(const Graph_t& graphNoDelay, const DelayGraph_t delayGraph, const Orders_t& orders, bool needPrint);
        Pipeline(const Graph_t& graphNoDelay, const Orders_t& orders, bool needPrint);
        Pipeline(const Graph_t& graphNoDelay, bool needPrint);
        ~Pipeline();
        // TODO: add func to check in_fmt, out_fmt, in_bitDepth, out_bitDepth????
        // TODO: run-time log of in_fmt, out_fmt, in_bitDepth, out_bitDepth????
        void generate_arg_cfg_template(int startFrameInd = 0, int frameNum = 1);
        void make_arg_cfg_file_multi_frames(const char* refFilePath, int startFrameInd = 0, int frameNum = 1);
        void default_run_pipe();
        void frames_run_pipe(Hash_t* pHsAll, int startFrameInd, int frameNum);
        void frames_run_pipe(const char* filePath, int startFrameInd, int frameNum);
        void dump();

    protected:
        void move_output_to_pool();
        const ImgPtrs_t distribute_in_img_t(const Module_t& sModule); // distribute who is main img, who is additional img, ...
        void sign_out_from_pool(const Module_t& sModule); // after input img is used, remove module name from deliverTo list; if list is empty, destroy the img.
        void signature_output_img(const Module_t& sModule); // assemble the img and signature
        void run_module(const Module_t& sModule, Hash_t* pHs);
        void run_pipe(Hash_t* pHsAll); // run pipeline for a single frame
        
    protected:
        MODULE_NAME* _sorted;
        Pipe_t _pipe;
        PipeImg_t _sOutPipeImg;
        int _frameInd = 0;

    private:
        std::vector<PipeImg_t> _InImgPool;
        void clear_imgs();
        Hash_t _subHsOneFrame;
        Hash_t get_sub_hash_one_frame_from_modules();
        std::string _configFrameStr = "FRAME #";
        std::string _baseCfgFilePath = "../args/base.toml";
        std::string _baseCfgInfo = 
            "\ninfo:\nthe base config file for the pipeline is generated at '" + _baseCfgFilePath + "'.\n"
            "you may copy it and modify the argument values to make your own config file,\n"
            "but do NOT change the file structure or argument names.\n\n"; 
            // TODO: add title, date time, add comments as instruction (what to do if...)
        // TODO: add offline mode, load some offline img
        // TODO: const char* namePrefix ???

};

void test_pipeline();
void test_pipeline2();

#endif