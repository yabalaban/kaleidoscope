//
//  lexer.hpp
//  kaleidoscope
//
//  Created by Alexander Balaban on 05/05/2023.
//

#ifndef lexer_hpp
#define lexer_hpp

#include <string>

enum class Token {
    kEof,
    
    // commands
    kDef,
    kExtern,
    
    // primary
    kIdentifier,
    kNumber,
};

static std::string identifier;  // Filled in if Token::kIdentifier
static double number;           // Filled in if Token::kNumber

extern auto GetToken() -> std::variant<Token, char>;

#endif /* lexer_hpp */
