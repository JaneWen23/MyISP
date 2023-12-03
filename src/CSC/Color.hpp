#ifndef _COLOR_H
#define _COLOR_H

#include "../Infra/ImgDef.hpp"
#include "../Filter/SlidingWindow.hpp"

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

void ccm_prototype();
void rgb_to_yuv420_prototype();

#endif