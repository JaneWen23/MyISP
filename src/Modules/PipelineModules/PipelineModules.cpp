#include "PipelineModules.hpp"


std::function<IMG_RTN_CODE(const Img_t*, Img_t*, const void*)> find_func_to_run(MODULE_ENUM m){
    switch (m){
        case ISP_VIN:{
            return read_raw_frame;
        }
        case ISP_COMPRESSION:{
            return my_jxs_pipe_sim;
        }
        case ISP_BLC:{
            break;
        }
        case ISP_DMS:{
            break;
        }
        case ISP_WB:{
            break;
        }
        case ISP_CCM:{
            return ccm;
        }
        case ISP_RGB2YUV:{
            break;
        }
        default:{
            std::cout<<"error: cannot find function for the module. exited.\n";
            exit(1);
        }
    }
    return read_raw_frame; // this is nonsense. just because it needs a return value.
}


Module_t config_isp_module(PipeCfg_t& sPipeCfg, PipeCfg_t& sPipeCfgLast){ // TODO: or "isp_module_cfg" ?
    Module_t sVinModule ={
        sPipeCfg.module,
        sPipeCfgLast.outFmt,
        sPipeCfg.outFmt,
        sPipeCfgLast.outBitDepth,
        sPipeCfg.outBitDepth,
        sPipeCfgLast.outSign,
        sPipeCfg.outSign,
        find_func_to_run(sPipeCfg.module)
    };
    return sVinModule;
}


void test_pipeline_modules(){

}