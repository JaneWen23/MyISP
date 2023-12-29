#include <iostream>
#include "common.hpp"

const char* get_module_name(const MODULE_NAME m){
    switch(m){
        case DUMMY0:{
            return "DUMMY0";
        }
        case DUMMY1:{
            return "DUMMY1";
        }
        case DUMMY2:{
            return "DUMMY2";
        }
        case DUMMY3:{
            return "DUMMY3";
        }
        case DUMMY4:{
            return "DUMMY4";
        }
        case DUMMY5:{
            return "DUMMY5";
        }
        case DUMMY6:{
            return "DUMMY6";
        }
        case DUMMY7:{
            return "DUMMY7";
        }
        case DUMMY8:{
            return "DUMMY8";
        }
        case ISP_VIN:{
            return "ISP_VIN";
        }
        case ISP_COMPRESSION:{
            return "ISP_COMPRESSION";
        }
        case ISP_BLC:{
            return "ISP_BLC";
        }
        case ISP_DMS:{
            return "ISP_DMS";
        }
        case ISP_WB:{
            return "ISP_WB";
        }
        case ISP_CCM:{
            return "ISP_CCM";
        }
        case ISP_RGB2YUV:{
            return "ISP_RGB2YUV";
        }
        default:{
            return "UNKNOWN ISP MODULE";
        }
    }
    return "";
}


IMG_RTN_CODE isp_dummy(const ImgPtrs_t sInImgPtrs, Img_t* pOutImg, void* pMArg_Dummy){
    std::cout<<"dummy module is running.\n";
    
    return SUCCEED;
}