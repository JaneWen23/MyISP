#ifndef _MATH_H
#define _MATH_H
#include <stdlib.h>

template<typename T>
const T dot_product(const T* a, const T* b, const int vecLen){
    T res = 0;
    int i = 0;
    // parallel computation, maybe
    for (i = 0; i < vecLen - 4; i += 4){
        res += (*(a+i)) * (*(b+i));
        res += (*(a+i+1)) * (*(b+i+1));
        res += (*(a+i+2)) * (*(b+i+2));
        res += (*(a+i+3)) * (*(b+i+3));
    }
    while(i < vecLen){
        res += (*(a+i)) * (*(b+i));
        ++i;
    }
    return res;
}

template<typename T>
const T dot_product(const T** a, const T** b, const int vecLen){ // in use
    // "T** a" is to help handel the padded signal.
    // "T** b" is to help handel the flipped kernel.
    T res = 0;
    int i = 0;
    // parallel computation, maybe
    for(i = 0; i < vecLen - 4; i += 4){
        res += (**(a+i)) * (**(b+i));
        res += (**(a+i+1)) * (**(b+i+1));
        res += (**(a+i+2)) * (**(b+i+2));
        res += (**(a+i+3)) * (**(b+i+3));
    }
    while(i < vecLen){
        res += (**(a+i)) * (**(b+i));
        ++i;
    }
    return res;
}

// this function is for test only. can be removed.
template<typename T>
const T my_formula(const T** a, const T** b, const int vecLen){
    T res = 0;
    for(int i = 0; i < vecLen; ++i){
        res += (**(a+i) > **(b+i))? **(a+i) : **(b+i);
    }
    return res;
}

template<typename T>
const T LeGall53_fwd_predict(const T** a, const T** b = NULL, const int vecLen = 3){
    return **(a+1) - ((**a + **(a+2))>>1);
}

template<typename T>
const T LeGall53_fwd_update(const T** a, const T** b = NULL, const int vecLen = 3){
    return **(a+1) + ((**a + **(a+2))>>2);
}

template<typename T>
const T LeGall53_bwd_predict(const T** a, const T** b = NULL, const int vecLen = 3){
    return **(a+1) + ((**a + **(a+2))>>1);
}

template<typename T>
const T LeGall53_bwd_update(const T** a, const T** b = NULL, const int vecLen = 3){
    return **(a+1) - ((**a + **(a+2))>>2);
}


template<typename T>
struct Formulas_T{
    const T (*f)(const T**, const T**, const int);
}; // note: cannot typedef, because there is no type yet.


template<typename T>
const T color_correction(const T** a, const T** colorMatRows, const int idx){
    // "**a" is the addr of channel 0 (panel 0).
    return (**(a) * (*colorMatRows[idx]) + **(a+1) * (*(colorMatRows[idx]+1)) + **(a+2) * (*(colorMatRows[idx]+2))) >> 8;
}

template<typename T>
const T rgb_to_yuv_bt709(const T** a, const T** b, const int idx){
    // "**a" is the addr of channel 0 (panel 0).
    const T colorMatRow1[3] = {66, 129, 25};
    const T colorMatRow2[3] = {-38, -74, 112};
    const T colorMatRow3[3] = {112, -94, -18};
    const T delta[3] = {128, 128, 128}; // may use another name
    const T offsets[3] = {16, 218, 128}; // may use another name
    const T* colorMatRows[3] = {colorMatRow1, colorMatRow2, colorMatRow3};
    return ((**(a) * (*colorMatRows[idx]) + **(a+1) * (*(colorMatRows[idx]+1)) + **(a+2) * (*(colorMatRows[idx]+2)) + delta[idx]) >> 8) + offsets[idx];
}

template<typename T>
struct Formulas_1x1_T{
    const T (*f1x1)(const T**, const T**, const int);
}; // note: cannot typedef, because there is no type yet.

#endif