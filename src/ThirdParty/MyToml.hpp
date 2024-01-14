#ifndef MY_TOML_H
#define MY_TOML_H

#include <toml.hpp>
#include "../Modules/COMMON/common.hpp"

void generate_tbl_from_hash(toml::v3::table* pTbl, Hash_t* pMyHash);
void generate_toml_file_from_hash(const char* fileName, Hash_t* pMyHash);
void set_hash_from_tbl(toml::v3::table* pTbl, Hash_t* pMyHash);

void test_toml();
#endif