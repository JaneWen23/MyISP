#ifndef _PIPELINE_MODULES_H
#define _PIPELINE_MODULES_H

// this file describes the bridge connecting Modules and Pipeline.
// PipelineModules are the actual modules used in pipeline,
// and they are "packages" of other modules in Modules.

#include <functional>
#include "common.hpp"
#include "../../Modules/ISP_VIN/Vin.hpp"
#include "../../Modules/ISP_COMPRESSION/MyJXS.hpp"
#include "../../Modules/ISP_CCM/ccm.hpp"


typedef struct{
    // defines the structure of pipeline; does not care about data and arguments.
    // minimal definition for people to write in TOML.
    MODULE_ENUM module;
    IMAGE_FMT inFmt;
    IMAGE_FMT outFmt;
    int inBitDepth;
    int outBitDepth;

} PipeUnit_t; // TODO: or change name to pipe unit min info???

#define MAX_NUM_NODE_IO 4

typedef struct{
    MODULE_ENUM module;
    MODULE_ENUM pred_modules[MAX_NUM_NODE_IO]; // predecessor modules
    MODULE_ENUM succ_modules[MAX_NUM_NODE_IO]; // successor modules

    // example: A,B,C,D:
    // A -> C,  B -> C, A -> D, C -> D
    // dependency: (the predecessor nodes)
    // A_depend = {},
    // B_depend = {},
    // C_depend = {A, B},
    // D_depend = {A, C}
    // who will need: (the successor nodes)
    // A_needed_by = {C, D},
    // B_needed_by = {C}
    // C_needed_by = {D}
    // D_needed_by = {}
} pipeNode;

// add func to take in pipeNode objects and generate a DAG, or a topology sorted vector

typedef struct{
    // this struct is usually inferred from PipeUnit_t;
    // i.e., full definition.
    MODULE_ENUM module;
    IMAGE_FMT inFmt;
    IMAGE_FMT outFmt;
    int inBitDepth;
    int outBitDepth;

    // TODO: module cfg will be carried out in pipeline class constructor
 
    std::function<IMG_RTN_CODE(const ImgPtrs_t, Img_t*, void*)> run_function;
} Module_t;

// Update 20231218
// forget the above. need to rethink about the modules in pipeline
// it is certain that a pipeline topology is a DAG (Directional Acyclic Graph)
// a module in a DAG is a node, so,
// a module may have uncertain number of input images
// and a module output may be used by multiple modules
// "module's function is different than Algo's function"
// module's interface:
// still generate one output Img_t (shared pointer), but input img is in list
// about the args: "module arg" may contain algo arg and more, for example, some adaptive args between two algos.
// and the args should contain the items to be memorized, like, adaptive n-tap filter coefficients => n numbers should be in args.
// modules should also take care of the adaptation of args during the running of stream,
// so, the module function may contain: 1, execution of this frame, and 2, update of args.
// so, the output should be an outImg and updatedArg (maybe update arg in the same place)

// the first input image should be the "main" image that being active during whole pipeline processing
// the pipeline should contain one main image and some buffered images. 
// only in the context of pipeline, need people to consider where to store the images.
// the inFmt, outFmt, ... : modules do not contain these info, but need to check every input's fmt and output fmt
// some algo are restricted to a certain fmt, so module needs to ensure that fmt is correct.
// modules i/o should be unsigned only. need to convert to unsigned if algo produces signed results.
// this gives rise to the problem of bit-depth conversion, since signed <==> unsigned may be complicated.
// and bit depth conversion may or may not cause the change of data type in cpp.
// however, we need to take care of these two cases.

// the pipeline itself may not determine "pipeline bit depth" independently; the bit depth should be determined by
// combination of sensor bit depth and each module's format and sign(defined by algo).
// pipeline bit depth is just a result of the modules' bit depth.

// what algos can be packed into a module: algos in serial, or just the one algo;
// two parallel algos should not be packed into one module, they should be two modules with proper names.
// modules may not contain the info about next or last module
// pipeline at construction: check in fmt of this module and out fmt of the last module. (what if two input formats?)

// every output has to be clear that which module(s) will be using it.
// and every output image should record the module name who made the output.
// if two modules will be using it, just record the names in list.
// when a new module is to be executed, the output image pointer should be "delivered" to "input pool", output ptr will be reset;
// and then "tells" the input image, now it is module xxx using the input image.
// after module xxx used image, the module name should be removed from list.
// after all modules finished using input images (list is empty), the input image will be freed.
// if an input image will be used latter, but will not be used by the next module:
// since the input image is not freed, just deliver the next module's input to another ptr in input pool.
// if a module needs two or more inputs, since every output knows who made it and there is dependency list, 
// we can get the corresponding inputs from the named-outputs.

Module_t generate_isp_module(PipeUnit_t& sPipeUnit);
void test_pipeline_modules();

#endif