#include <iostream>
#include "parse.hpp"

void parse_args(const char* pathFile, const int frameInd){ //TODO: consider a series of frames' args
    toml::table tbl = toml::parse_file(pathFile);
}


// TODO: to be removed:
// void set_tbl_from_hash(toml::v3::table* pTbl, Hash_t* pMyHash){
//     // this function works ONLY IF the tbl and the hash has the same structure!
//     for (auto&& [k, v] : (*pTbl)){
//         if (v.is_value()){
//             std::string kStr = k.data();
//             if ((*pMyHash).at(kStr).type() == typeid(int)){
//                 (*pTbl).insert_or_assign(k, std::any_cast<int>((*pMyHash).at(kStr)));
//             }
//             else if ((*pMyHash).at(kStr).type() == typeid(std::string)){
//                 (*pTbl).insert_or_assign(k, std::any_cast<std::string>((*pMyHash).at(kStr)));
//             }
//             else if ((*pMyHash).at(kStr).type() == typeid(const char*)){
//                 (*pTbl).insert_or_assign(k, std::any_cast<const char*>((*pMyHash).at(kStr)));
//             }
//             else if ((*pMyHash).at(kStr).type() == typeid(bool)){
//                 (*pTbl).insert_or_assign(k, std::any_cast<bool>((*pMyHash).at(kStr)));
//             }
//             else {
//                 std::cout<<"error: the type of value is not supported at key = "<<kStr<<", exited.\n";
//                 exit(1);
//             }
//         }
//         else{
//             Hash_t* pMySubHash = std::any_cast<Hash_t>(&((*pMyHash).at(k.data())));
//             set_tbl_from_hash(v.as_table(), pMySubHash);
//         }
//     }
// }

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