#include <iostream>
#include "parse.hpp"

void parse_args(const char* pathFile, const int frameInd){
    toml::table tbl = toml::parse_file(pathFile);
}

ParsedArgs::ParsedArgs(const char* pathFile){
    _tbl = toml::parse_file(pathFile);
}

ParsedArgs::~ParsedArgs(){
    // nothing to do
}

int ParsedArgs::size(){
    return _tbl.size();
}

void ParsedArgs::sort_root_keys(std::string rootKeyIndicator){ // not yet in use
    for (auto it = _tbl.begin(); it != _tbl.end(); ++it){
        std::string kStr = (*it).first.data();
        if (kStr.find(rootKeyIndicator) != std::string::npos){
            _sortedKeys.insert(kStr);
        }
        else{
            std::cout<<"error: the provided config file contains invalid root-level key; should be like "<< rootKeyIndicator<<"0. exited.\n";
            exit(1);
        }
    }
}

void ParsedArgs::fetch_args_at_frame(std::string rootKey, Hash_t* pHs){
    // hash <==> sub table under the root key
    override_hash_from_tbl(_tbl.at(rootKey).as_table(), pHs);
}


void test_toml_to_hash(){
    toml::table tbl = toml::parse_file("../args/sample.toml");

    auto pSbTbl = tbl["FRAME #0"].as_table();

    (*pSbTbl).for_each([](const toml::key& key, auto&& val){
        std::cout << key << ": " << val << "\n";
    });

}