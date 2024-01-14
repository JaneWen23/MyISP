#include "PipelineModules.hpp"



std::function<IMG_RTN_CODE(const ImgPtrs_t, Img_t*, Hash_t*)> find_func_for_module(MODULE_NAME m){
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

Hash_t default_dummy_arg_hash(){
    return {{"a", 3}};
}
MArg_Dummy_t get_dummy_arg_struct_from_hash(Hash_t* pHs){
    int a = std::any_cast<int>((*pHs).at("a"));
    return {
        a
    };
}

Hash_t get_default_arg_hash_for_module(const MODULE_NAME m){
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
            return default_dummy_arg_hash();
        }
        case ISP_VIN:{
            return default_vin_arg_hash();
        }
        case ISP_COMPRESSION:{
            return default_compression_arg_hash();
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
            return default_ccm_arg_hash();
        }
        case ISP_RGB2YUV:{
            break;
        }
        default:{
            std::cout<<"error: cannot find default hash table for the module. exited.\n";
            exit(1);
        }
    }
    return {{"nan", 0}};
}

Hash_t* find_arg_hash_for_module(Hash_t* pHsAll, const MODULE_NAME m){
    std::string moduleName = get_module_name(m);
    Hash_t* pHs = std::any_cast<Hash_t>(&((*pHsAll).at(moduleName)));
    return pHs;
}

IMG_RTN_CODE isp_dummy(const ImgPtrs_t sInImgPtrs, Img_t* pOutImg, Hash_t* pHs){
    MArg_Dummy_t sMArg = get_dummy_arg_struct_from_hash(pHs);
    print_hash(pHs);
    std::cout<<"in img data p0 ptrs: ";
    if (sInImgPtrs.size() == 0){
        std::cout<<"N/A; ";
    }
    else{
        for (int i = 0; i < sInImgPtrs.size(); ++i){
            if (sInImgPtrs[i] != NULL){
                std::cout<< (void*)(sInImgPtrs[i]->pImageData[0])<< ", ";
            }
            else{
                std::cout<< "NULL, ";
            }
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
