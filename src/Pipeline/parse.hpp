#ifndef _PARSE_H
#define _PARSE_H

#include "../ThirdParty/MyToml.hpp"
#include "../Modules/COMMON/PipelineModules.hpp"
#include <set>


class ParsedArgs{ // this class is to wrap up the toml table. do not want pipeline.cpp to deal with toml table directly.
    public:
        ParsedArgs(const char* pathFile);
        ~ParsedArgs();
        int size();
        void sort_root_keys(std::string rootKeyIndicator);
        void fetch_args_at_frame(std::string rootKey, Hash_t* pHs);
    private:
        toml::v3::table _tbl;
        std::set<std::string> _sortedKeys;
};

void test_parse_args();
void test_toml_to_hash();
void test_hash_to_toml();
#endif