#ifndef _DUMMY_H
#define _DUMMY_H

#include "../COMMON/common.hpp"

typedef struct{
    int a;
} MArg_Dummy_t;

Hash_t default_dummy_arg_hash();
MArg_Dummy_t get_dummy_arg_struct_from_hash(Hash_t* pHs);
IMG_RTN_CODE isp_dummy(const ImgPtrs_t sInImgPtrs, Img_t* pOutImg, Hash_t* pMArg_Dummy, bool updateArgs);

#endif