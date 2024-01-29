#ifndef _PARSE_H
#define _PARSE_H

#include "../ThirdParty/MyToml.hpp"
#include "../Modules/COMMON/PipelineModules.hpp"
#include "PipeCommon.hpp"
#include <set>


class ParsedArgs{ // this class is to wrap up the toml table. do not want pipeline.cpp to deal with toml table directly.
    public:
        ParsedArgs(const char* file);
        ~ParsedArgs();
        int size();
        void sort_root_keys(std::string rootKeyIndicator);
        void fetch_args_at_frame(std::string rootKey, Hash_t* pHs);
    private:
        toml::v3::table _tbl;
        std::set<std::string> _sortedKeys;
};

class ParsedPipe{
    public:
        ParsedPipe(const char* file);
        ~ParsedPipe();
        std::unordered_map<std::string, bool> get_info() const;
        Graph_t get_graph_no_delay() const;
        DelayGraph_t get_graph_with_delay() const;
        Orders_t get_input_orders() const;
    private:
        toml::v3::table _tbl;
        std::unordered_map<std::string, bool> _info;
};

void test_parse_args();
void test_toml_to_hash();
void test_hash_to_toml();
void test_parsed_pipe();
#endif