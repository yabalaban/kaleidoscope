//
//  parser.hpp
//  kaleidoscope
//
//  Created by Alexander Balaban on 05/05/2023.
//

#ifndef parser_hpp
#define parser_hpp

#include <map>
#include "lexer.hpp"

extern std::map<char, int> binop_precedence;

extern auto MainLoop() -> void;

#endif /* parser_hpp */
