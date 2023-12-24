#include "ccm.hpp"

IMG_RTN_CODE isp_ccm(const ImgPtrs_t sInImgPtrs, Img_t* pOutImg, void* pMArg_CCM){
    MArg_CCM_t* pMArg = (MArg_CCM_t*)pMArg_CCM;
    if (sInImgPtrs.pMainImg->imageFormat != RGB){
        std::cout<<"error: ISP_CCM: input image not being RGB. exited.\n";
        exit(1);
    }
    safe_unsigned_to_signed_img(sInImgPtrs.pMainImg); // ccm() is defined to be signed.

    // TODO: may design MArg CCM elements to be all positive ==> negatives are in complement,
    // and then convert them to the signed CCMArg.

    ccm(sInImgPtrs.pMainImg, pOutImg, &(pMArg->sCCMArg));
    if (pOutImg->imageFormat != RGB){
        std::cout<<"error: ISP_CCM: output image not being RGB. exited.\n";
        exit(1);
    }
    
    safe_signed_to_unsigned_img(pOutImg); // modules only allow unsigned data.
    
    // TODO: add function to update the color matrix

    return SUCCEED;
}