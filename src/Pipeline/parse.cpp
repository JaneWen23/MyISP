#include <iostream>
#include "parse.hpp"

void parse_args(const char* pathFile, const int frameInd, AllArgs_t& sArgs){

    toml::table tbl = toml::parse_file(pathFile);

    // toml::path the_path("sVinArg.sAlgoVinArg");
    // auto node = tbl[the_path];
    // ReadRawArg_t sAlgoVinArg = {
    //     node["path"].value<decltype(sAlgoVinArg.path)>().value(),
    //     node["frameInd"].value<decltype(sAlgoVinArg.frameInd)>().value(), //int frameInd; // read i-th frame, i >= 0, WILL BE UPDATED IN THE RUN-TIME; if rewind = true, this will not be updated
    //     get_image_format_from_name(node["imageFormat"].value<const char*>().value()), //IMAGE_FMT imageFormat;
    //     node["width"].value<decltype(sAlgoVinArg.width)>().value(), //int width;
    //     node["height"].value<decltype(sAlgoVinArg.height)>().value(), //int height;
    //     node["bitDepth"].value<decltype(sAlgoVinArg.bitDepth)>().value(), //int bitDepth;
    //     node["alignment"].value<decltype(sAlgoVinArg.alignment)>().value(), //int alignment;
    // };
    // sArgs.sVinArg = {sAlgoVinArg, tbl["sVinArg"]["rewind"].value<bool>().value()};

    // toml::path the_path2("sCompressionArg.sStarTetrixArg");
    // auto node2 = tbl[the_path2];
    // StarTetrixArg_t sStarTetrixArg = {
    //     node2["Wr"].value<int>().value(), // int Wr
    //     node2["Wb"].value<int>().value() //int Wb
    // };

    // toml::path the_path3("sCompressionArg.sDWTArg");
    // auto node3 = tbl[the_path3];
    // DWTArg_t sDWTArg = {
    //     node3["inImgPanelId"].value<int>().value(), // int inImgPanelId;
    //     node3["outImgPanelId"].value<int>().value(), // int outImgPanelId;
    //     get_dwt_orient_from_name(node3["orient"].value<const char*>().value()), // ORIENT orient;
    //     node3["level"].value<int>().value(), // int level;
    //     get_wavelet_from_name(node3["wavelet"].value<const char*>().value()), // WAVELET_NAME wavelet;
    //     get_padding_from_name(node3["padding"].value<const char*>().value()) // PADDING padding;
    // };
    
    // sArgs.sCompressionArg = {sStarTetrixArg, sDWTArg};

    // toml::path the_path4("sCCMArg.sAlgoCCMArg");
    // auto node4 = tbl[the_path4];
    // CCMArg_t sAlgoCCMArg = {
    //     {node4[0][0].value<int>().value(), node4[0][1].value<int>().value(), node4[0][2].value<int>().value()}, //{278, -10, -8},
    //     {node4[1][0].value<int>().value(), node4[1][1].value<int>().value(), node4[1][2].value<int>().value()}, //{-12, 269, -8},
    //     {node4[2][0].value<int>().value(), node4[2][1].value<int>().value(), node4[2][2].value<int>().value()}, //{-10, -3, 272},
    // };
    // sArgs.sCCMArg = {sAlgoCCMArg};

}

void print_parsed_args(AllArgs_t& sArgs){


    std::cout<<"sArgs.sVinArg = {\n";
    std::cout<<"sAlgoVinArg = {\n";
    std::cout<<"const char* path = "<<sArgs.sVinArg.sReadRawArg.path<<"\n";
    std::cout<<"int frameInd = "<<sArgs.sVinArg.sReadRawArg.frameInd<<"\n";
    std::cout<<"IMAGE_FMT imageFormat = "<<get_image_format_name(sArgs.sVinArg.sReadRawArg.imageFormat)<<"\n";
    std::cout<<"int width = "<<sArgs.sVinArg.sReadRawArg.width<<"\n";
    std::cout<<"int height = "<<sArgs.sVinArg.sReadRawArg.height<<"\n";
    std::cout<<"int bitDepth = "<<sArgs.sVinArg.sReadRawArg.bitDepth<<"\n";
    std::cout<<"int alignment = "<<sArgs.sVinArg.sReadRawArg.alignment<<"\n";
    std::cout<<"}\n";
    std::cout<<"bool rewind = "<<sArgs.sVinArg.rewind<<"\n";
    std::cout<<"}\n";

    std::cout<<"sArgs.sCompressionArg = {\n";
    std::cout<<"sStarTetrixArg = {\n";
    std::cout<<"int Wr = "<<sArgs.sCompressionArg.sStarTetrixArg.Wr<<"\n";
    std::cout<<"int Wb = "<<sArgs.sCompressionArg.sStarTetrixArg.Wb<<"\n";
    std::cout<<"}\n";
    std::cout<<"sDWTArg = {\n";
    std::cout<<"int inImgPanelId = "<<sArgs.sCompressionArg.sDWTArg.inImgPanelId<<"\n";
    std::cout<<"int outImgPanelId = "<<sArgs.sCompressionArg.sDWTArg.outImgPanelId<<"\n";
    std::cout<<"ORIENT orient = "<<get_dwt_orient_name(sArgs.sCompressionArg.sDWTArg.orient)<<"\n";
    std::cout<<"int level = "<<sArgs.sCompressionArg.sDWTArg.level<<"\n";
    std::cout<<"WAVELET_NAME wavelet = "<<get_wavelet_name(sArgs.sCompressionArg.sDWTArg.wavelet)<<"\n";
    std::cout<<"PADDING padding = "<<get_padding_name(sArgs.sCompressionArg.sDWTArg.padding)<<"\n";
    std::cout<<"}\n";
    std::cout<<"}\n";

    std::cout<<"sArgs.sCCMArg = {\n";
    std::cout<<"sAlgoCCMArg = {\n";
    // std::cout<<sArgs.sCCMArg.sCCMArg.colorMatRow1[0]<<", "<<sArgs.sCCMArg.sCCMArg.colorMatRow1[1]<<", "<<sArgs.sCCMArg.sCCMArg.colorMatRow1[2]<<"\n";
    // std::cout<<sArgs.sCCMArg.sCCMArg.colorMatRow2[0]<<", "<<sArgs.sCCMArg.sCCMArg.colorMatRow2[1]<<", "<<sArgs.sCCMArg.sCCMArg.colorMatRow2[2]<<"\n";
    // std::cout<<sArgs.sCCMArg.sCCMArg.colorMatRow3[0]<<", "<<sArgs.sCCMArg.sCCMArg.colorMatRow3[1]<<", "<<sArgs.sCCMArg.sCCMArg.colorMatRow3[2]<<"\n";
    std::cout<<"}\n";
    std::cout<<"}\n";
}

void test_parse_args(){
    AllArgs_t sArgs = {};
    parse_args("../args/sample.toml", 0, sArgs);
    print_parsed_args(sArgs);
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

    toml::path the_path("sVinArg");
    auto pSbTbl = tbl[the_path].as_table();

      
    // (*pSbTbl).for_each([](const toml::key& key, auto&& val){
    //     std::cout << key << ": " << val << "\n";
    // });

    Hash_t myHash; // a hash "template". the "largest" hash.

    Hash_t hs = default_vin_arg_hash();
    Hash_t hs2 = default_ccm_arg_hash();
    Hash_t hs3 = default_compression_arg_hash();

    myHash = {{"sVinArg", hs}, {"sCompressionArg", hs3}, {"sCCMArg", hs2}};

    //set_hash_from_tbl(pSbTbl, &myHash);
    print_hash(&myHash);


    // set_tbl_from_hash(pSbTbl, &myHash);
    // (*pSbTbl).for_each([](const toml::key& key, auto&& val){
    //     std::cout << key << ": " << val << "\n";
    // });

}