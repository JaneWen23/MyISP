#include <iostream>
#include "parse.hpp"

void parse_args(const char* pathFile, const int frameInd){
    toml::table tbl = toml::parse_file(pathFile);
}


void set_hash_from_tbl(toml::v3::table* pTbl, Hash_t* pMyHash){
    // this function works ONLY IF the tbl and the hash has the same structure!
    for (auto&& [k, v] : (*pTbl)){
        if (v.is_value()){
            std::string kStr = k.data();
            if ((*pMyHash).at(kStr).type() == typeid(int)){
                (*pMyHash).at(kStr) = v.value<int>().value();
            }
            else if ((*pMyHash).at(kStr).type() == typeid(std::string)){
                (*pMyHash).at(kStr) = v.value<std::string>().value();
            }
            else if ((*pMyHash).at(kStr).type() == typeid(const char*)){
                (*pMyHash).at(kStr) = v.value<const char*>().value();
            }
            else if ((*pMyHash).at(kStr).type() == typeid(bool)){
                (*pMyHash).at(kStr) = v.value<bool>().value();
            }
            else {
                std::cout<<"error: the type of value is not supported at key = "<<kStr<<", exited.\n";
                exit(1);
            }
        }
        else{
            std::string kStr = k.data();
            Hash_t* pMySubHash = std::any_cast<Hash_t>(&((*pMyHash).at(kStr)));
            set_hash_from_tbl(v.as_table(), pMySubHash);
        }
    }
}

void set_tbl_from_hash(toml::v3::table* pTbl, Hash_t* pMyHash){
    // this function works ONLY IF the tbl and the hash has the same structure!
    for (auto&& [k, v] : (*pTbl)){
        if (v.is_value()){
            std::string kStr = k.data();
            if ((*pMyHash).at(kStr).type() == typeid(int)){
                (*pTbl).insert_or_assign(k, std::any_cast<int>((*pMyHash).at(kStr)));
            }
            else if ((*pMyHash).at(kStr).type() == typeid(std::string)){
                (*pTbl).insert_or_assign(k, std::any_cast<std::string>((*pMyHash).at(kStr)));
            }
            else if ((*pMyHash).at(kStr).type() == typeid(const char*)){
                (*pTbl).insert_or_assign(k, std::any_cast<const char*>((*pMyHash).at(kStr)));
            }
            else if ((*pMyHash).at(kStr).type() == typeid(bool)){
                (*pTbl).insert_or_assign(k, std::any_cast<bool>((*pMyHash).at(kStr)));
            }
            else {
                std::cout<<"error: the type of value is not supported at key = "<<kStr<<", exited.\n";
                exit(1);
            }
        }
        else{
            Hash_t* pMySubHash = std::any_cast<Hash_t>(&((*pMyHash).at(k.data())));
            set_tbl_from_hash(v.as_table(), pMySubHash);
        }
    }
}

void generate_tbl_from_hash(toml::v3::table* pTbl, Hash_t* pMyHash){
    for (auto it = pMyHash->begin(); it != pMyHash->end(); ++it){
        std::string key = (*it).first;
        if ((*it).second.type() != typeid(Hash_t)){
            if ((*it).second.type() == typeid(int)){
                (*pTbl).emplace(key, std::any_cast<int>((*it).second));
            }
            else if ((*it).second.type() == typeid(std::string)){
                (*pTbl).emplace(key, std::any_cast<std::string>((*it).second));
            }
            else if ((*it).second.type() == typeid(const char*)){
                (*pTbl).emplace(key, std::any_cast<const char*>((*it).second));
            }
            else if ((*it).second.type() == typeid(bool)){
                (*pTbl).emplace(key, std::any_cast<bool>((*it).second));
            }
            else {
                std::cout<<"error: the type of value is not supported at key = "<<key<<", exited.\n";
                exit(1);
            }
        }
        else{
            std::string key = (*it).first;
            toml::v3::table subTbl;
            (*pTbl).emplace(key, subTbl);
            Hash_t* pMySubHash = std::any_cast<Hash_t>(&((*it).second));
            generate_tbl_from_hash((*pTbl).at(key).as_table(), pMySubHash);
        }
    }
}

void generate_toml_file_from_hash(const char* fileName, Hash_t* pMyHash){
    toml::v3::table tbl;
    generate_tbl_from_hash(&tbl, pMyHash);
    std::ofstream out(fileName);
    if (out.is_open()){
        out<<tbl;
        out.close();
    }
}

void test_hash_to_toml(){

    MODULE_NAME mods[3] = {ISP_VIN, ISP_COMPRESSION, ISP_CCM};
    Hash_t allArgsHash;
    allArgsHash.insert({"ISP_VIN", get_default_arg_hash_for_module(mods[0])});
    allArgsHash.insert({"ISP_COMPRESSION", get_default_arg_hash_for_module(mods[1])});
    allArgsHash.insert({"ISP_CCM", get_default_arg_hash_for_module(mods[2])});
    print_hash(&allArgsHash);

    toml::v3::table tbl;
    generate_tbl_from_hash(&tbl, &allArgsHash);

    tbl.for_each([](const toml::key& key, auto&& val){
        std::cout << key << ": " << val << "\n";
    });

    generate_toml_file_from_hash("../dump/base.toml", &allArgsHash);
}

void test_toml_to_hash(){
    toml::table tbl = toml::parse_file("../args/sample.toml");

    toml::path the_path("Vin");
    auto pSbTbl = tbl[the_path].as_table();

      
    // (*pSbTbl).for_each([](const toml::key& key, auto&& val){
    //     std::cout << key << ": " << val << "\n";
    // });

    Hash_t myHash; // a hash "template". the "largest" hash.

    Hash_t hs = default_vin_arg_hash();
    Hash_t hs2 = default_ccm_arg_hash();
    Hash_t hs3 = default_compression_arg_hash();

    myHash = {{"Vin", hs}, {"Compression", hs3}, {"CCM", hs2}};

    //set_hash_from_tbl(pSbTbl, &myHash);
    print_hash(&myHash);


    // set_tbl_from_hash(pSbTbl, &myHash);
    // (*pSbTbl).for_each([](const toml::key& key, auto&& val){
    //     std::cout << key << ": " << val << "\n";
    // });

}