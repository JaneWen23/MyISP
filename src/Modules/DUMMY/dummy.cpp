#include "dummy.hpp"
#include <iostream>

Hash_t default_dummy_arg_hash(){
    return {{"a", 3}};
}

MArg_Dummy_t get_dummy_arg_struct_from_hash(Hash_t* pHs){
    int a = std::any_cast<int>((*pHs).at("a"));
    return {
        a
    };
}

IMG_RTN_CODE isp_dummy(const ImgPtrs_t sInImgPtrs, Img_t* pOutImg, Hash_t* pHs, bool updateArgs){
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

    std::cout<<"out img data p0 ptr: ";
    std::cout<< (void*)(pOutImg->pImageData[0])<< "\n";

    if (updateArgs){
        set_hash_at_path(pHs, {"a"}, sMArg.a + 1);
    }

    return SUCCEED;
}