#include <iostream>
#include "MyToml.hpp"

void test_toml(){
    toml::table tbl = toml::parse_file( "../args/sample.toml" );
    std::cout << tbl << "\n";
}