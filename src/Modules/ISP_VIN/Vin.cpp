# include "Vin.hpp"

IMG_RTN_CODE isp_vin(const ImgPtrs_t sInImgPtrs, Img_t* pOutImg, void* pMArg_Vin){
    MArg_Vin_t* pMArg = (MArg_Vin_t*)pMArg_Vin;
    // Input Img: don't care.
    read_raw_to_img_t(pMArg->sReadRawArg.path,
                       pOutImg,
                       pMArg->sReadRawArg.imageFormat,
                       pMArg->sReadRawArg.width,
                       pMArg->sReadRawArg.height,
                       pMArg->sReadRawArg.bitDepth, // bitDepth is at least sensor bitDepth + 1 ?????????
                       pMArg->sReadRawArg.alignment,
                       pMArg->sReadRawArg.frameInd);

    pOutImg->sign = UNSIGNED; // just to make sure that it is unsigned.

    if (!(pMArg->rewind)){
        pMArg->sReadRawArg.frameInd += 1;
    }
    
    return SUCCEED;
}