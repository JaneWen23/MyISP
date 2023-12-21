#include "ccm.hpp"

IMG_RTN_CODE isp_ccm(const ImgPtrs_t sImgPtrs, Img_t* pOutImg, void* pMArg_CCM){
    MArg_CCM_t* pMArg = (MArg_CCM_t*)pMArg_CCM;
    if (sImgPtrs.pMainImg->imageFormat != RGB){
        std::cout<<"error: input image not being RGB. exited.\n";
        exit(1);
    }
    sImgPtrs.pMainImg->sign = SIGNED; // ccm() is defined to be signed.

    // TODO: may design MArg CCM elements to be all positive ==> negatives are in complement,
    // and then convert them to the signed CCMArg.

    ccm(sImgPtrs.pMainImg, pOutImg, &(pMArg->sCCMArg));
    if (pOutImg->imageFormat != RGB){
        std::cout<<"error: output image not being RGB. exited.\n";
        exit(1);
    }
    
    pOutImg->sign = UNSIGNED; // modules only allow unsigned data.

    // TODO: add function to update the color matrix

    return SUCCEED;
}