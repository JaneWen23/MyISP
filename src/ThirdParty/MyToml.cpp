#include <iostream>
#include <fstream>
#include "MyToml.hpp"

void test_toml(){
    toml::table tbl = toml::parse_file( "../args/sample.toml" );
    std::cout << tbl["sVinArg"]["rewind"] << "\n";
    std::cout << tbl["sVinArg"]["sAlgoVinArg"] << "\n";
    std::cout << tbl["sCCMArg"]["sAlgoCCMArg"][0] << "\n";
    std::cout << tbl["sCCMArg"]["sAlgoCCMArg"][1] << "\n";
    std::cout << tbl["sCCMArg"]["sAlgoCCMArg"][2] << "\n";
    std::cout << tbl << "\n";

    std::ofstream out("../dump/out.toml");
    if (out.is_open()){
        out<<tbl;
        out.close();
    }

}