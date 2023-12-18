#ifndef _PIPELINE_MODULES_H
#define _PIPELINE_MODULES_H

// this file describes the bridge connecting Modules and Pipeline.
// PipelineModules are the actual modules used in pipeline,
// and they are "packages" of other modules in Modules.

#include <functional>
#include "../Algos/Infra/ImgDef.hpp"
#include "../Algos/Infra/ImageIO.hpp"
#include "../Algos/Compression/MyJXS.hpp"
#include "../Algos/Color/Color.hpp"

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
    std::function<IMG_RTN_CODE(const Img_t*, Img_t*, const void*)> run_function; //TODO: what if a module needs two input images?
    // TODO: maybe, module function is different than function's function
} Module_t;

// Update 20231218
// forget the above. need to rethink about the modules in pipeline
// it is certain that a pipeline topology is a DAG (Directional Acyclic Graph)
// a module in a DAG is a node, so,
// a module may have uncertain number of inputs (i.e., input images and their arguments)
// and a module output may be used by multiple modules
// "module's function is different than Algo's function"
// need to design module's interface
// still generate one output Img_t (shared pointer), but input img and arg are in lists
// the execution of a module is just the function as said above
// the first input image should be the "main" image that being active during whole pipeline processing
// the pipeline should contain one main image and some buffered images. 
// only in the context of pipeline, need people to consider where to store the images.
// modules may also contain inFmt, outFmt, inBitDepth, outBitDepth, inSign, outSign
// modules may not contain the info about next or last module
// in the module interface, check_input_fmt() and check_output_fmt() must be performed.
// 

Module_t generate_isp_module(PipeUnit_t& sPipeUnit, PipeUnit_t& sPipeUnitLast);
void test_pipeline_modules();

#endif