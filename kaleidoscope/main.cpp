//
//  main.cpp
//  kaleidoscope
//
//  Created by Alexander Balaban on 05/05/2023.
//

#include <iostream>
#include "lexer.hpp"
#include "parser.hpp"

int main(int argc, const char * argv[]) {
    binop_precedence['<'] = 10;
    binop_precedence['+'] = 20;
    binop_precedence['-'] = 20;
    binop_precedence['*'] = 40;
    MainLoop();
    return 0;
}
