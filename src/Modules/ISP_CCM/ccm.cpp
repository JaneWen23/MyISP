#include "ccm.hpp"

Hash_t default_ccm_arg_hash(){
    int colorMatRow1[3] = {278, -10, -8};
    int colorMatRow2[3] = {-12, 269, -8};
    int colorMatRow3[3] = {-10, -3, 272};
    Hash_t Row1 = {
        {"0", colorMatRow1[0]},
        {"1", colorMatRow1[1]},
        {"2", colorMatRow1[2]}
    };
    Hash_t Row2 = {
        {"0", colorMatRow2[0]},
        {"1", colorMatRow2[1]},
        {"2", colorMatRow2[2]}
    };
    Hash_t Row3 = {
        {"0", colorMatRow3[0]},
        {"1", colorMatRow3[1]},
        {"2", colorMatRow3[2]}
    };

    Hash_t CCMArg = {
        {"colorMatRow1", Row1},
        {"colorMatRow2", Row2},
        {"colorMatRow3", Row3}
    };
    Hash_t hs = {{"CCMArg", CCMArg}};
    return hs;
}

IMG_RTN_CODE isp_ccm(const ImgPtrs_t sInImgPtrs, Img_t* pOutImg, void* pMArg_CCM){
    MArg_CCM_t* pMArg = (MArg_CCM_t*)pMArg_CCM;
    if (sInImgPtrs[0]->imageFormat != RGB){
        std::cout<<"error: ISP_CCM: input image not being RGB. exited.\n";
        exit(1);
    }
    safe_unsigned_to_signed_img(sInImgPtrs[0]); // ccm() is defined to be signed.

    // TODO: may design MArg CCM elements to be all positive ==> negatives are in complement,
    // and then convert them to the signed CCMArg.

    ccm(sInImgPtrs[0], pOutImg, &(pMArg->sCCMArg));
    if (pOutImg->imageFormat != RGB){
        std::cout<<"error: ISP_CCM: output image not being RGB. exited.\n";
        exit(1);
    }
    
    safe_signed_to_unsigned_img(pOutImg); // modules only allow unsigned data.
    
    // TODO: add function to update the color matrix

    return SUCCEED;
}

void test_ccm_hash(){
    Hash_t hs = default_ccm_arg_hash();
    print_hash(&hs);
}