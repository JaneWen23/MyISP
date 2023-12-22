#include "common.hpp"

const char* get_module_name(const MODULE_ENUM m){
    switch(m){
        case ISP_NONE:{
            return "ISP_NONE";
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

void signed_to_unsigned_img(Img_t* pImg){
    // convert signed img to unsigned img while bit depth remains unchanged. (not always!!!!)
    // usually used when module finished processing an image,
    // since the processed image may be signed and pipeline only allow
    // data in range [0, 2^bitDepth - 1], we should first clamp then set sign to UNSIGNED.

    // clamp: if clamp really happens, print a warning.

    
}

void unsigned_to_signed_img(Img_t* pImg){
    // this is usually used when module just got the img from pipeline,
    // in the case that the algo needs signed img, 
    // module should convert it to SIGNED before pass to algo.

    // for example, uint12 to int12
    // if any uint12 values > 2^11-1, they will be 2^11-1(?)

    // if sensor bit depth is 12, we need bitDepth >= 13 to make sure signed data type also carries data.
    // 
    // if sensor bit depth is 12, when it becomes signed, we need 13 bit,
    // and in cpp program, they are both treated as 16 bit signed / unsigned, so, "data type bit depth" does not change
    // that's what it means by "convert unsigned img to signed img while bit depth remains unchanged".
    // if sensor bit depth is 8, cpp treats it as 8 bit unsigned,
    // if it is converted to signed, we need 9 bit signed, and cpp treats it as 16 bit signed,
    // so we need a function to convert img from 8 bit to 16 bit in this case
}
