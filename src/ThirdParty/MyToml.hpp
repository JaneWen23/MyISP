#ifndef MY_TOML_H
#define MY_TOML_H

#include <toml.hpp>
#include "../Modules/COMMON/common.hpp"

void generate_toml_file_from_hash(const char* fileName, Hash_t* pMyHash);
void override_hash_from_toml_file(const char* filePath, Hash_t* pMyHash, const std::string rootKeyIndicator);

void test_toml();
#endif