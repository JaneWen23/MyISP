#ifndef _PARSE_H
#define _PARSE_H

#include "../ThirdParty/MyToml.hpp"
#include "../Modules/COMMON/PipelineModules.hpp"


void parse_args(const int frameInd, AllArgs_t& sArgs);
void test_parse_args();
void test_toml_to_hash();
#endif