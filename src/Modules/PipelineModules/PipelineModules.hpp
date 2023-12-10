#ifndef _PIPELINE_MODULES_H
#define _PIPELINE_MODULES_H

// this file describes the bridge connecting Modules and Pipeline.
// PipelineModules are the actual modules used in pipeline,
// and they are "packages" of other modules in Modules.

#include <functional>
#include "../Infra/ImgDef.hpp"
#include "../Infra/ImageIO.hpp"
#include "../Compression/MyJXS.hpp"
#include "../CSC/Color.hpp"

typedef enum {
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
    MODULE_ENUM module;
    IMAGE_FMT outFmt;
    int outBitDepth;
    SIGN outSign; // TODO: usually UNSIGNED in pipeline. it's just that some of my modules have sign.
} PipeCfg_t; // TODO: maybe another name, like user module cfg, do not use pipe. pipe should be an array of this.

typedef struct{
    // this struct is usually inferred from PipeCfg_t
    MODULE_ENUM module;
    IMAGE_FMT inFmt;
    IMAGE_FMT outFmt;
    int inBitDepth;
    int outBitDepth;
    SIGN inSign;
    SIGN outSign;
    std::function<IMG_RTN_CODE(const Img_t*, Img_t*, const void*)> run_function;
} Module_t;


Module_t config_isp_module(PipeCfg_t& sPipeCfg, PipeCfg_t& sPipeCfgLast); // must match with the isp module

#endif