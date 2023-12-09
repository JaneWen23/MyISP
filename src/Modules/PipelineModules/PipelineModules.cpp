#include "PipelineModules.hpp"

Module_t isp_vin_module_cfg(Args_t& sArgs){
    Module_t sVinModule ={
        ISP_VIN,
        RAW_RGGB,
        RAW_RGGB,
        16,
        16,
        UNSIGNED,
        UNSIGNED,
        &(sArgs.sVinArg),
        read_raw_frame
    };
    return sVinModule;
}

Module_t isp_compression_module_cfg(Args_t& sArgs){
    Module_t sCompressionModule ={
        ISP_VIN,
        RAW_RGGB,
        RAW_RGGB,
        16,
        16,
        SIGNED, // TODO: may need some converter, from unsigned to signed
        SIGNED,
        &(sArgs.sVinArg), // TODO: modify;
        read_raw_frame // TODO: modify;
    };
    return sCompressionModule;
}


void test_pipeline_modules(){

    ReadRawArg_t sVinArg = {
        "../data/rawData.raw", //const char* path;
        0, //int frameInd; // read i-th frame, i >= 0, WILL BE UPDATED IN THE RUN-TIME; if rewind = true, this will not be updated
        RAW_RGGB, //IMAGE_FMT imageFormat;
        4256, //int width;
        2848, //int height;
        16, //int bitDepth;
        1, //int alignment;
    };
    ReadRawArg_t* pVinArg = &sVinArg;   


    StarTetrixArg_t sStarTetrixArg = {
        1,
        2
    };
    StarTetrixArg_t* pStarTetrixArg = &sStarTetrixArg;
    Module_t sStarTetrixModule = {
        ISP_VIN,
        RAW_RGGB,
        RAW_RGGB,
        16,
        16,
        UNSIGNED,
        UNSIGNED,
        (void*)pStarTetrixArg,
        star_tetrix_forward
    };


    DWTArg_t* pDWTArg = (DWTArg_t*)malloc(sizeof(DWTArg_t));
    pDWTArg->level = 2;
    pDWTArg->orient = TWO_DIMENSIONAL;
    pDWTArg->inImgPanelId = 0;
    pDWTArg->outImgPanelId = 0;
    config_dwt_kernels_LeGall53<int>(pDWTArg, MIRROR);
    DWTArg_t sDWTArg = *pDWTArg;


    CCMArg_t sCCMArg = {
        {278, -10, -8},
        {-12, 269, -8},
        {-10, -3, 272},
    };
    CCMArg_t* pCCMArg = &sCCMArg;


    Args_t sArgs = {
        // DO NOT DO THIS:
        // {
        //     "../data/rawData.raw", //const char* path;
        //     0, //int frameInd; // read i-th frame, i >= 0, WILL BE UPDATED IN THE RUN-TIME; if rewind = true, this will not be updated
        //     RAW_RGGB, //IMAGE_FMT imageFormat;
        //     4256, //int width;
        //     2848, //int height;
        //     16, //int bitDepth;
        //     1, //int alignment;
        // },

        // {
        //     1,
        //     2
        // }

        // DO THIS:
        sVinArg,
        sStarTetrixArg,
        sDWTArg,
        sCCMArg
    };
}