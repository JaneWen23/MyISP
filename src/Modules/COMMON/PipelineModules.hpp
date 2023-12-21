#ifndef _PIPELINE_MODULES_H
#define _PIPELINE_MODULES_H

// this file describes the bridge connecting Modules and Pipeline.
// PipelineModules are the actual modules used in pipeline,
// and they are "packages" of other modules in Modules.

#include <functional>
#include "common.hpp"
#include "../../Algos/Infra/ImageIO.hpp"
#include "../../Modules/ISP_VIN/Vin.hpp"
#include "../../Modules/ISP_COMPRESSION/MyJXS.hpp"
#include "../../Modules/ISP_CCM/ccm.hpp"

typedef enum {
    ISP_NONE,
    ISP_VIN,
    ISP_COMPRESSION,
    ISP_BLC,
    ISP_DMS,
    ISP_WB,
    ISP_CCM,
    ISP_RGB2YUV
} MODULE_ENUM;


typedef struct{
    // defines the structure of pipeline; does not care about data and arguments.
    // minimal definition for people to write in TOML.
    MODULE_ENUM module;
    MODULE_ENUM lastModule;//?????????
    IMAGE_FMT outFmt;
    int outBitDepth;
    SIGN outSign; // TODO: usually UNSIGNED in pipeline. it's just that some of my modules have sign.
} PipeUnit_t; // TODO: or change name to pipe unit min info???

typedef struct{
    // this struct is usually inferred from PipeUnit_t;
    // i.e., full definition, or redundant definition.
    MODULE_ENUM module;
    MODULE_ENUM lastModule;
    //MODULE_ENUM nextModule; // TODO: TBD
    // or pointers to other input/output than the class members _input _output // TODO: TBD
    // pointers will be specified at module cfg; and module cfg will be carried out in pipeline class constructor // TODO: TBD
    IMAGE_FMT inFmt;
    IMAGE_FMT outFmt;
    int inBitDepth;
    int outBitDepth;
    SIGN inSign;
    SIGN outSign;
    std::function<IMG_RTN_CODE(const ImgPtrs_t, Img_t*, void*)> run_function; //TODO: what if a module needs two input images?
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

// what algos can be packed into a module: algos in serial, or just the one algo;
// two parallel algos should not be packed into one module, they should be two modules with proper names.
// modules may not contain the info about next or last module
// if pipeline needs 12 bits but algo needs more bits and sign: when algo finished, convert back to 12-bit unsigned.
// pipeline at construction: check in fmt of this module and out fmt of the last module. (what if two input formats?)

Module_t generate_isp_module(PipeUnit_t& sPipeUnit, PipeUnit_t& sPipeUnitLast);
void test_pipeline_modules();

#endif