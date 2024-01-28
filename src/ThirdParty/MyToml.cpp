#include <iostream>
#include <fstream>
#include "MyToml.hpp"

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
            // else if ((*it).second.type() == typeid(const char*)){
            //     (*pTbl).emplace(key, std::any_cast<const char*>((*it).second));
            // }
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

void override_hash_from_tbl(toml::v3::table* pTbl, Hash_t* pMyHash){
    // this function works ONLY IF the tbl and the hash have the same structure!
    for (auto&& [k, v] : (*pTbl)){
        if (v.is_value()){
            std::string kStr = k.data();
            if ((*pMyHash).at(kStr).type() == typeid(int)){
                (*pMyHash).at(kStr) = v.value<int>().value();
            }
            else if ((*pMyHash).at(kStr).type() == typeid(std::string)){
                (*pMyHash).at(kStr) = v.value<std::string>().value();
            }
            // else if ((*pMyHash).at(kStr).type() == typeid(const char*)){
            //     (*pMyHash).at(kStr) = v.value<std::string>().value(); // type cast as std::string to avoid the use-after-free problem
            // }
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
            override_hash_from_tbl(v.as_table(), pMySubHash); 
        }
    }
}

// void override_hash_from_toml_file(const char* filePath, Hash_t* pMyHash, const std::string rootKeyIndicator){
//     toml::table tbl = toml::parse_file(filePath);
    
//     // check validity: find if exact "root" key is in table.
//     auto it = tbl.begin();
//     std::string kStr = (*it).first.data();
//     if (kStr.find(rootKeyIndicator) != std::string::npos){
//         // pMyHash <==> sub table under the root key!!
//         override_hash_from_tbl(tbl.at(kStr).as_table(), pMyHash);
//     }
//     else{
//         std::cout<<"error: the provided config file does not have a valid root-level key, like "<< rootKeyIndicator<<"0. exited.\n";
//         exit(1);
//     }
// }

void test_toml(){
    toml::table tbl = toml::parse_file( "../args/sample.toml" );
    std::cout << tbl << "\n";

    std::ofstream out("../dump/out.toml");
    if (out.is_open()){
        out<<tbl;
        out.close();
    }

}

void test_toml2(){
    toml::table tbl2 = toml::parse_file( "../pipeCfg/pipeCfgDummy2.toml" );
    // std::cout << tbl2 << "\n";
    // std::ofstream out2("../dump/outPipeCfg2.toml");
    // if (out2.is_open()){
    //     out2<<tbl2;
    //     out2.close();
    // }

    // check if a specific key is in the table:
    std::cout<<tbl2.contains("graphNoDelay")<<"\n";
    std::cout<<tbl2.contains("delayGraph")<<"\n";
    std::cout<<tbl2.contains("orders")<<"\n";
    std::cout<<tbl2.contains("aaa")<<"\n";

    //toml::v3::array* pA = tbl2["graphNoDelay"].as_array();
    //toml::array& arr = *tbl2.get_as<toml::array>("graphNoDelay");

    // graph:
    std::cout<<"graph:\n";
    for(int i = 0; i <(*tbl2["graphNoDelay"].as_array()).size(); ++i){
        std::string tmp = tbl2["graphNoDelay"][i][0].value<std::string>().value();
        std::cout<<"module: "<<tmp<<", ";
        std::cout<<"succNoDelay: ";
        for(int j = 0; j <(*tbl2["graphNoDelay"][i][1].as_array()).size(); ++j){
            std::cout<<tbl2["graphNoDelay"][i][1][j].value<std::string>().value()<<", ";
        }
        std::cout<<"\n";
    }
    std::cout<<"\n";
        
    // delay_graph:
    std::cout<<"delayGraph:\n";
    for(int i = 0; i < (*tbl2["delayGraph"].as_array()).size(); ++i){
        std::string tmp = tbl2["delayGraph"][i][0].value<std::string>().value();
        std::cout<<"module: "<<tmp<<", ";
        std::cout<<"succWithDelay: ";
        for(int j = 0; j <(*tbl2["delayGraph"][i][1].as_array()).size(); ++j){
            std::cout<<tbl2["delayGraph"][i][1][j][0].value<std::string>().value()<<", ";
            std::cout<<tbl2["delayGraph"][i][1][j][1].value<int>().value()<<", ";
        }
        std::cout<<"\n";
    }
    std::cout<<"\n";

    // orders:
    std::cout<<"orders:\n";
    for(int i = 0; i < (*tbl2["orders"].as_array()).size(); ++i){
        std::string tmp = tbl2["orders"][i][0].value<std::string>().value();
        std::cout<<"module: "<<tmp<<", ";
        std::cout<<"input in order: ";
        for(int j = 0; j <(*tbl2["orders"][i][1].as_array()).size(); ++j){
            std::cout<<tbl2["orders"][i][1][j][0].value<std::string>().value()<<", ";
            if((*tbl2["orders"][i][1][j].as_array()).size() == 2){
                std::cout<<tbl2["orders"][i][1][j][1].value<int>().value()<<", ";
            }
        }
        std::cout<<"\n";
    }
    std::cout<<"\n";
   
}