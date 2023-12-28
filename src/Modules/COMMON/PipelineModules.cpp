#include "PipelineModules.hpp"



std::function<IMG_RTN_CODE(const ImgPtrs_t, Img_t*, void*)> find_func_for_module(MODULE_NAME m){
    switch (m){
        case DUMMY0:
        case DUMMY1:
        case DUMMY2:
        case DUMMY3:
        case DUMMY4:
        case DUMMY5:
        case DUMMY6:
        case DUMMY7:
        case DUMMY8:{
            return isp_dummy;
        }
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





void test_pipeline_modules(){

}