#ifndef _MY_JXS_H
#define _MY_JXS_H

#include "../COMMON/common.hpp"
#include "../../Algos/StarTetrix/StarTetrixTrans.hpp"
#include "../../Algos/DWT/dwt.hpp"


typedef struct{
    StarTetrixArg_t sStarTetrixArg;
    DWTArg_t sDWTArg;
    // more...
} MArg_Compression_t;


Hash_t default_compression_arg_hash();
const MArg_Compression_t get_compression_arg_struct_from_hash(Hash_t* pHs);
IMG_RTN_CODE isp_compression(const ImgPtrs_t sImgPtrs, Img_t* pOutImg, Hash_t* pMyJXSArg); // this is forward + backward

void test_my_jxs();

#endif