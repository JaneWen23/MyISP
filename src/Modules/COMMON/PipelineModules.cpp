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


void* find_arg_for_func(const AllArgs_t& sArgs, const MODULE_NAME m){ // TODO: may rewrite as hash table version, find sub hash from big hash, by key = module name
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
            return (void*)(&(sArgs.sDummyArg));
        }
        case ISP_VIN:{
            return (void*)(&(sArgs.sVinArg));
        }
        case ISP_COMPRESSION:{
            return (void*)(&(sArgs.sCompressionArg));
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
            return (void*)(&(sArgs.sCCMArg));
        }
        case ISP_RGB2YUV:{
            break;
        }
        default:{
            std::cout<<"error: cannot find arguments for this function to run. exited.\n";
            exit(1);
        }
    }
    return NULL; // this is nonsense. just because it needs a return value.
}

IMG_RTN_CODE isp_dummy(const ImgPtrs_t sInImgPtrs, Img_t* pOutImg, void* pMArg_Dummy){
    std::cout<<"dummy module is running.\n";
    std::cout<<"in img data p0 ptrs: ";
    for (int i = 0; i < sInImgPtrs.size(); ++i){
        if (sInImgPtrs[i] != NULL){
            std::cout<< (void*)(sInImgPtrs[i]->pImageData[0])<< ", ";
        }
        else{
            std::cout<< "NULL, ";
        }
    }

    IMAGE_FMT imageFormat = RGB;
    size_t width = 6;
    size_t height = 4;
    size_t bitDepth = 16;
    size_t alignment = 32;
    construct_img(pOutImg, 
                imageFormat,
                width,
                height,
                UNSIGNED,
                bitDepth,
                alignment,
                true);
    Distrib_t sDistrib = {0, 511, 3, 15};  
    ValCfg_t sValCfg = {SIGNED, rand_num_uniform, sDistrib};
    set_value(pOutImg, sValCfg);

    std::cout<<"out img data p0 ptr: ";
    std::cout<< (void*)(pOutImg->pImageData[0])<< "\n";

    return SUCCEED;
}
