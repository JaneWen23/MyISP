#ifndef _PIPELINE_MODULES_H
#define _PIPELINE_MODULES_H

// integration of all modules

#include <functional>

#include "../../Modules/ISP_VIN/Vin.hpp"
#include "../../Modules/ISP_COMPRESSION/MyJXS.hpp"
#include "../../Modules/ISP_CCM/ccm.hpp"

typedef struct{
    int a;
} MArg_Dummy_t;

typedef struct{
    // the integration of all module arguments (beginning with "MArg")
    MArg_Vin_t sVinArg;
    MArg_Compression_t sCompressionArg;
    MArg_CCM_t sCCMArg;
    MArg_Dummy_t sDummyArg;
} AllArgs_t; // TOML // TODO: replace by hash table


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

// if two inputs: which one is main img, which one is additional img?
// may need an "instruction" in cfg file

Hash_t default_dummy_arg_hash();
std::function<IMG_RTN_CODE(const ImgPtrs_t, Img_t*, Hash_t*)> find_func_for_module(MODULE_NAME m);
Hash_t get_default_arg_hash_for_module(const MODULE_NAME m);
Hash_t* find_arg_hash_for_module(Hash_t* pHsAll, const MODULE_NAME m);


IMG_RTN_CODE isp_dummy(const ImgPtrs_t sInImgPtrs, Img_t* pOutImg, Hash_t* pMArg_Dummy);

//Module_t generate_isp_module(PipeUnit_t& sPipeUnit);

#endif