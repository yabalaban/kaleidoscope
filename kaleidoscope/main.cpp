//
//  main.cpp
//  kaleidoscope
//
//  Created by Alexander Balaban on 05/05/2023.
//

#include <iostream>
#include "lexer.hpp"

int main(int argc, const char * argv[]) {
    auto token = GetToken();
    std::visit([](const auto &x) { std::cout << static_cast<std::underlying_type<Token>::type>(x) ; }, token);
    std::cout << "\n";
    return 0;
}
