#ifndef _PIPELINE_MODULES_H
#define _PIPELINE_MODULES_H

// this file describes the bridge connecting Modules and Pipeline.
// PipelineModules are the actual modules used in pipeline,
// and they are "packages" of other modules in Modules.

#include <functional>
#include "../Infra/ImgDef.hpp"
#include "../Infra/ImageIO.hpp"
#include "../Infra/RandImageGen.hpp"
#include "../DWT/dwt.hpp"
#include "../Filter/SlidingWindow.hpp"
#include "../StarTetrix/StarTetrixTrans.hpp"
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
    MODULE_ENUM module;
    IMAGE_FMT inFmt;
    IMAGE_FMT outFmt;
    int inBitDepth;
    int outBitDepth;
    SIGN inSign;
    SIGN outSign;
    void* pArg;
    std::function<IMG_RTN_CODE(Img_t*, Img_t*, void*)> run_function;
} Module_t;

typedef struct{ // interation of the Arg_t's; does not necessarily match the isp modules
    ReadRawArg_t sVinArg;
    StarTetrixArg_t sStarTetrixArg;
    DWTArg_t sDWTArg;
    // may add compression arg, then delete above two.
    CCMArg_t sCCMArg;
} Args_t;

Module_t isp_vin_module_cfg(Args_t& sArgs); // must match the isp module
Module_t isp_compression_module_cfg(Args_t& sArgs);
Module_t isp_blc_module_cfg(Args_t& sArgs);
Module_t isp_dms_module_cfg(Args_t& sArgs);
Module_t isp_wb_module_cfg(Args_t& sArgs);
Module_t isp_ccm_module_cfg(Args_t& sArgs);
Module_t isp_rgb2yuv_module_cfg(Args_t& sArgs);

#endif