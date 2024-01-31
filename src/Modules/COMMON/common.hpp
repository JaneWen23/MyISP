#ifndef _COMMON_H
#define _COMMON_H

#include "../../Algos/Infra/ImgDef.hpp"
#include <vector>
#include <unordered_map>
#include <any>

typedef std::vector<Img_t*> ImgPtrs_t;
// since a module can have one or more input images, store the img pointers in the vector.
// for example, 
//      ImgPtrs_t p(4);
// means p[0] is the pointer to main img, p[1] additional img1, p[2] additional img2, ...

typedef enum {
    DUMMY0,
    DUMMY1,
    DUMMY2,
    DUMMY3,
    DUMMY4,
    DUMMY5,
    DUMMY6,
    DUMMY7,
    DUMMY8,
    ISP_VIN,
    ISP_COMPRESSION,
    ISP_BLC,
    ISP_DMS,
    ISP_WB,
    ISP_CCM,
    ISP_RGB2YUV
} MODULE_NAME;

typedef std::unordered_map<std::string, std::any> Hash_t;
std::string get_module_name(const MODULE_NAME m);
MODULE_NAME get_module_from_name(std::string name);
void print_hash(Hash_t* pMyHash, const char* title = "");
void set_hash_at_path(Hash_t* pHs, std::vector<std::string> hsPath, std::any val);

#endif