#ifndef _PARSE_H
#define _PARSE_H

#include "../ThirdParty/MyToml.hpp"
#include "../Modules/COMMON/PipelineModules.hpp"


void parse_args(const int frameInd);
void generate_toml_file_from_hash(const char* fileName, Hash_t* pMyHash);

void test_parse_args();
void test_toml_to_hash();
void test_hash_to_toml();
#endif