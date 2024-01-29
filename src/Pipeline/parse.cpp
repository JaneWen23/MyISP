#include <iostream>
#include "parse.hpp"

// void parse_args(const char* file, const int frameInd){
//     toml::table tbl = toml::parse_file(file);
// }

ParsedArgs::ParsedArgs(const char* file){
    _tbl = toml::parse_file(file);
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



ParsedPipe::ParsedPipe(const char* file){
    _tbl = toml::parse_file(file);
    if(_tbl.contains("graphNoDelay")){
        _info.insert({"graphNoDelay", true});
    }
    else{
        _info.insert({"graphNoDelay", false});
    }

    if(_tbl.contains("delayGraph")){
        _info.insert({"delayGraph", true});
    }
    else{
        _info.insert({"delayGraph", false});
    }

    if(_tbl.contains("orders")){
        _info.insert({"orders", true});
    }
    else{
        _info.insert({"orders", false});
    }
}

ParsedPipe::~ParsedPipe(){
    // do nothing
}

// returns info about what kind of graphs the cfg has
std::unordered_map<std::string, bool> ParsedPipe::get_info() const{
    return _info;
}

Graph_t ParsedPipe::get_graph_no_delay() const{
    if( ! _info.at("graphNoDelay")){
        std::cout<<"error: the parsed pipeline config does not contain 'graphNoDelay'. exited.\n";
        exit(1);
    }
    Graph_t graph;
    for(int i = 0; i <(*_tbl["graphNoDelay"].as_array()).size(); ++i){
        std::string moduleName = _tbl["graphNoDelay"][i][0].value<std::string>().value();
        std::vector<MODULE_NAME> succModules;
        for(int j = 0; j <(*_tbl["graphNoDelay"][i][1].as_array()).size(); ++j){
            std::string succModule = _tbl["graphNoDelay"][i][1][j].value<std::string>().value();
            if(strcmp(succModule.c_str(), "") != 0){
                succModules.push_back(get_module_from_name(succModule));
            }
        }
        graph.push_back({get_module_from_name(moduleName), succModules});
    }
    return graph;
}

DelayGraph_t ParsedPipe::get_graph_with_delay() const{
    if( ! _info.at("delayGraph")){
        std::cout<<"error: the parsed pipeline config does not contain 'delayGraph'. exited.\n";
        exit(1);
    }
    DelayGraph_t delayGraph;
    for(int i = 0; i < (*_tbl["delayGraph"].as_array()).size(); ++i){
        std::string moduleName = _tbl["delayGraph"][i][0].value<std::string>().value();
        std::vector<ModuleDelay_t> succWthDelay;
        for(int j = 0; j <(*_tbl["delayGraph"][i][1].as_array()).size(); ++j){
            std::string succModuleName = _tbl["delayGraph"][i][1][j][0].value<std::string>().value();
            int delay = _tbl["delayGraph"][i][1][j][1].value<int>().value();
            succWthDelay.push_back({get_module_from_name(succModuleName), delay});
        }
        delayGraph.push_back({get_module_from_name(moduleName), succWthDelay});
    }
    return delayGraph;
}

Orders_t ParsedPipe::get_input_orders() const{
    if( ! _info.at("orders")){
        std::cout<<"error: the parsed pipeline config does not contain 'orders'. exited.\n";
        exit(1);
    }
    Orders_t orders;
    for(int i = 0; i < (*_tbl["orders"].as_array()).size(); ++i){
        std::string moduleName = _tbl["orders"][i][0].value<std::string>().value();
        std::vector<ModuleDelay_t> inputInOrder;
        for(int j = 0; j <(*_tbl["orders"][i][1].as_array()).size(); ++j){
            std::string inputModuleName = _tbl["orders"][i][1][j][0].value<std::string>().value();
            int delay = 0; // default
            if((*_tbl["orders"][i][1][j].as_array()).size() == 2){
                delay = _tbl["orders"][i][1][j][1].value<int>().value(); // override
            }
            inputInOrder.push_back({get_module_from_name(inputModuleName), delay});
        }
        orders.push_back({get_module_from_name(moduleName), inputInOrder});
    }
    return orders;
}


void test_toml_to_hash(){
    toml::table tbl = toml::parse_file("../args/sample.toml");

    auto pSbTbl = tbl["FRAME #0"].as_table();

    (*pSbTbl).for_each([](const toml::key& key, auto&& val){
        std::cout << key << ": " << val << "\n";
    });

}

void test_parsed_pipe(){
    ParsedPipe myParsedPipe("../pipeCfg/pipeCfgDummy2.toml");
    // Graph_t graph = myParsedPipe.get_graph_no_delay();
    // DelayGraph_t delayGraph = myParsedPipe.get_graph_with_delay();
    // Orders_t orders = myParsedPipe.get_input_orders();
    std::cout<< myParsedPipe.get_info().at("graphNoDelay")<<", ";
    std::cout<< myParsedPipe.get_info().at("delayGraph")<<", ";
    std::cout<< myParsedPipe.get_info().at("orders")<<"\n ";
}