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
const T dot_product(const T** a, const T** b, const int vecLen){
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
struct Formulas_T{
    const T (*f)(const T**, const T**, const int);
}; // note: cannot typedef, because there is no type yet.


// TODO: find max, find min??

template<typename T>
const T clamp(const T val, const T lowerBound, const T upperBound){
    T tmp = val < lowerBound? lowerBound : val;
    return (tmp > upperBound? upperBound : tmp); 
}

template<typename T>
struct Clamp_T{
    const T (*f)(const T, const T, const T);
};

#endif