#include <iostream>
#include "common.hpp"
#include "../../Algos/Infra/RandImageGen.hpp"

const char* get_module_name(const MODULE_NAME m){
    switch(m){
        case DUMMY0:{
            return "DUMMY0";
        }
        case DUMMY1:{
            return "DUMMY1";
        }
        case DUMMY2:{
            return "DUMMY2";
        }
        case DUMMY3:{
            return "DUMMY3";
        }
        case DUMMY4:{
            return "DUMMY4";
        }
        case DUMMY5:{
            return "DUMMY5";
        }
        case DUMMY6:{
            return "DUMMY6";
        }
        case DUMMY7:{
            return "DUMMY7";
        }
        case DUMMY8:{
            return "DUMMY8";
        }
        case ISP_VIN:{
            return "ISP_VIN";
        }
        case ISP_COMPRESSION:{
            return "ISP_COMPRESSION";
        }
        case ISP_BLC:{
            return "ISP_BLC";
        }
        case ISP_DMS:{
            return "ISP_DMS";
        }
        case ISP_WB:{
            return "ISP_WB";
        }
        case ISP_CCM:{
            return "ISP_CCM";
        }
        case ISP_RGB2YUV:{
            return "ISP_RGB2YUV";
        }
        default:{
            return "UNKNOWN ISP MODULE";
        }
    }
    return "";
}

void print_hash(Hash_t* pMyHash){
    for (auto it = pMyHash->begin(); it != pMyHash->end(); ++it){
        if ((*it).second.type() != typeid(Hash_t)){
            std::cout<< " '"<< (*it).first <<"' = ";
            if ((*it).second.type() == typeid(int)){
                std::cout<<std::any_cast<int>((*it).second)<<"\n";
            }
            else if ((*it).second.type() == typeid(std::string)){
                std::cout<<std::any_cast<std::string>((*it).second)<<"\n";
            }
            else if ((*it).second.type() == typeid(const char*)){
                std::cout<<std::any_cast<const char*>((*it).second)<<"\n";
            }
            else if ((*it).second.type() == typeid(bool)){
                std::cout<<std::any_cast<bool>((*it).second)<<"\n";
            }
            else {
                std::cout<<"\nerror: the type of value is not supported at key = "<< (*it).first <<", exited.\n";
                exit(1);
            }
        }
        else{
            std::cout<< (*it).first<<": \n";
            Hash_t* pMySubHash = std::any_cast<Hash_t>(&((*it).second));
            print_hash(pMySubHash);
        }
    }
}