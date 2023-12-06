#ifndef _PIPELINE_H
#define _PIPELINE_H

#include<functional>
#include "../Modules/Infra/ImgDef.hpp"

typedef struct{
// 
} Pipeline_t;

typedef struct{
    std::function<IMG_RTN_CODE(Img_t*, Img_t*, void*)> run_function;
} Module_t;

#endif