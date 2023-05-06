//
//  lexer.cpp
//  kaleidoscope
//
//  Created by Alexander Balaban on 05/05/2023.
//

#include "lexer.hpp"

std::string identifier;
double number;

auto inline SkipWhitespaces(char *last) {
    while (isspace(*last)) {
        *last = getchar();
    }
}

auto inline SkipSingleComment(char *last) -> bool {
    if (*last != '#') {
        return false;
    }
    do {
        *last = getchar();
    } while (*last != EOF && *last != '\n' && *last != '\r');
    return *last != EOF;
}

auto inline TryIdentifier(char *last) -> std::optional<Token> {
    if (!isalpha(*last)) {
        return {};
    }
    
    identifier = *last;
    while (isalnum(*last = getchar())) {
        identifier += *last;
    }
    
    if (identifier == "def") {
        return Token::kDef;
    }
    if (identifier == "extern") {
        return Token::kExtern;
    }
    
    return Token::kIdentifier;
}

auto inline TryNumber(char *last) -> std::optional<Token> {
    if (!(isdigit(*last) || *last == '.')) {
        return {};
    }
    
    std::string num_str;
    do {
        num_str += *last;
        *last = getchar();
    } while (isdigit(*last) || *last == '.');
    
    number = strtod(num_str.c_str(), 0);
    return Token::kNumber;
}

auto GetToken() -> std::variant<Token, char> {
    static char last = ' ';
    
    SkipWhitespaces(&last);
    if (auto id_token = TryIdentifier(&last)) {
        return id_token.value();
    }
    if (auto number_token = TryNumber(&last)) {
        return number_token.value();
    }
    if (SkipSingleComment(&last)) {
        return GetToken();
    }
    
    if (last == EOF) {
        return Token::kEof;
    }
    
    char ch = last;
    last = getchar();
    return ch;
}
