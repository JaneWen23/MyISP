#include <iostream>
#include <fstream>
#include "MyToml.hpp"

void test_toml(){
    toml::table tbl = toml::parse_file( "../args/sample.toml" );
    std::cout << tbl["Vin"]["rewind"] << "\n";
    std::cout << tbl["Vin"]["ReadRawArg"] << "\n";
    std::cout << tbl["CCM"]["ReadRawArg"][0] << "\n";
    std::cout << tbl["CCM"]["ReadRawArg"][1] << "\n";
    std::cout << tbl["CCM"]["ReadRawArg"][2] << "\n";
    std::cout << tbl << "\n";

    std::ofstream out("../dump/out.toml");
    if (out.is_open()){
        out<<tbl;
        out.close();
    }

}