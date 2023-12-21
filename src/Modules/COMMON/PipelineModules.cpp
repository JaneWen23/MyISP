#include "PipelineModules.hpp"



std::function<IMG_RTN_CODE(const ImgPtrs_t, Img_t*, void*)> find_func_to_run(MODULE_ENUM m){
    switch (m){
        case ISP_VIN:{
            return isp_vin;
        }
        case ISP_COMPRESSION:{
            return isp_compression;
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
            return isp_ccm;
        }
        case ISP_RGB2YUV:{
            break;
        }
        default:{
            std::cout<<"error: cannot find function for the module. exited.\n";
            exit(1);
        }
    }
    std::cout<<"error: cannot find function for the module. exited.\n";
    exit(1);
}


// //TODO: maybe before the generate isp module, we can first determine who is the last, and who is the next!!
// Module_t generate_isp_module(PipeUnit_t& sPipeUnit, PipeUnit_t& sPipeUnitLast){
// // TODO: if sPipeCfgLast does not exist, should take care of it before pass to here.
//     Module_t sVinModule ={
//         sPipeUnit.module,
//         sPipeUnitLast.module,
//         sPipeUnitLast.outFmt,
//         sPipeUnit.outFmt,
//         sPipeUnitLast.outBitDepth,
//         sPipeUnit.outBitDepth,
//         sPipeUnitLast.outSign,
//         sPipeUnit.outSign,
//         find_func_to_run(sPipeUnit.module)
//     };
//     return sVinModule;
// }


void test_pipeline_modules(){

}