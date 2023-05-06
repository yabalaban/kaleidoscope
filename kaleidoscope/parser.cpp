//
//  parser.cpp
//  kaleidoscope
//
//  Created by Alexander Balaban on 05/05/2023.
//

#include "parser.hpp"
#include "ast.hpp"

std::map<char, int> binop_precedence;
std::variant<Token, char> current_token;

auto static inline GetChar() -> std::optional<char> {
    if (!std::holds_alternative<char>(current_token)) {
        return {};
    }
    return std::get<char>(current_token);
}

auto static inline GetTokenEnum() -> std::optional<Token> {
    if (!std::holds_alternative<Token>(current_token)) {
        return {};
    }
    return std::get<Token>(current_token);
}

auto static GetNextToken() -> std::variant<Token, char> {
    return current_token = GetToken();
}

auto static GetTokPrecedence() -> int {
    if (auto ch = GetChar()) {
        if (!isascii(ch.value())) {
            return -1;
        }
        
        int prec = binop_precedence[ch.value()];
        if (prec <= 0) return -1;
        return prec;
    }
    return -1;
}

auto static LogError(const char *str) -> std::unique_ptr<AST::Expr> {
    fprintf(stderr, "Error: %s\n", str);
    return nullptr;
}

auto static LogErrorP(const char *str) -> std::unique_ptr<AST::Prototype> {
    LogError(str);
    return nullptr;
}

// numberexpr ::= number
auto static ParseNumberExpr() -> std::unique_ptr<AST::Expr> {
    auto result = std::make_unique<AST::NumberExpr>(number);
    GetNextToken();
    return std::move(result);
}

auto static ParseExpression() ->  std::unique_ptr<AST::Expr>;

// parenexpr ::= '(' expression ')'
auto static ParseParenExpr() -> std::unique_ptr<AST::Expr> {
    GetNextToken(); // consume '('
    auto v = ParseExpression();
    if (!v) { return nullptr; }
    if (GetChar() != ')') {
        return LogError("expected ')'");
    }
    GetNextToken(); // consume ')'
    return v;
}

// identifierexpr
//   ::= identifier
//   ::= identifier '(' expression* ')'
auto static ParseIdentifierExpr() -> std::unique_ptr<AST::Expr> {
    std::string id_name = identifier;
    
    GetNextToken(); // consume identifier
    
    if (GetChar() != '(') { // variable
        return std::make_unique<AST::VariableExpr>(id_name);
    }
    
    std::vector<std::unique_ptr<AST::Expr>> args;
    GetNextToken(); // consume '('
    if (GetChar() != ')') { // skip 0-arg
        while (true) {
            if (auto arg = ParseExpression()) {
                args.push_back(std::move(arg));
            } else {
                return nullptr;
            }
            
            if (GetChar() == ')') {
                break;
            }
            if (GetChar() != ',') {
                return LogError("expected ')' or ',' in argument list");
            }

            GetNextToken(); // consume ','
        }
    }
    GetNextToken(); // consume ')'
    return std::make_unique<AST::CallExpr>(id_name, std::move(args));
}

// primary
//   ::= identifierexpr
//   ::= numberexpr
//   ::= parenexpr
auto static ParsePrimary() -> std::unique_ptr<AST::Expr> {
    if (auto token = GetTokenEnum()) {
        switch (token.value()) {
            case Token::kIdentifier:
                return ParseIdentifierExpr();
            case Token::kNumber:
                return ParseNumberExpr();
            default:
                break;
        }
    } else if (auto ch = GetChar()) {
        switch (ch.value()) {
            case '(':
                return ParseParenExpr();
            default:
                break;
        }
    }
    return LogError("unknown token when expecting an expression");
}

// binoprhs
//   ::= ('+' primary)*
auto static ParseBinOpRHS(int expr_prec, std::unique_ptr<AST::Expr> lhs) -> std::unique_ptr<AST::Expr> {
    while (true) {
        int prec = GetTokPrecedence();
        if (prec < expr_prec) {
            return lhs;
        }
        
        auto bin_op = GetChar();
        GetNextToken();
        
        auto rhs = ParsePrimary();
        if (!rhs) { return nullptr; }
        
        int next_prec = GetTokPrecedence();
        if (prec < next_prec) {
            rhs = ParseBinOpRHS(prec + 1, std::move(rhs));
            if (!rhs) { return nullptr; }
        }
        
        lhs = std::make_unique<AST::BinaryExpr>(bin_op.value(), std::move(lhs), std::move(rhs));
    }
}

// expressions
//   ::= primary binopr
auto static ParseExpression() -> std::unique_ptr<AST::Expr> {
    if (auto lhs = ParsePrimary()) {
        return ParseBinOpRHS(0, std::move(lhs));
    }
    return nullptr;
}

// prototype
//   ::= id '(' id* ')'
auto static ParsePrototype() -> std::unique_ptr<AST::Prototype> {
    auto token = GetTokenEnum();
    if (token && token.value() != Token::kIdentifier) {
        return LogErrorP("expected function name in prototype");
    }
    
    std::string fn_name = identifier;
    GetNextToken(); // consume id
    
    if (GetChar() != '(') {
        return LogErrorP("expected '(' in prototype");
    }
    
    GetNextToken(); // consume '('
    std::vector<std::string> arg_names;
    while (GetTokenEnum() == Token::kIdentifier) {
        arg_names.push_back(identifier);
        GetNextToken();
    }
    GetNextToken(); // consume ')'
    
    return std::make_unique<AST::Prototype>(fn_name, std::move(arg_names));
}

// definition ::= 'def' prototype expression
auto static ParseDefinition() -> std::unique_ptr<AST::Function> {
    GetNextToken(); // consume 'def'
    auto proto = ParsePrototype();
    if (!proto) { return nullptr; };
    
    if (auto e = ParseExpression()) {
        return std::make_unique<AST::Function>(std::move(proto), std::move(e));
    }
    return nullptr;
}

// external ::= 'extern' prototype
auto static ParseExtern() -> std::unique_ptr<AST::Prototype> {
    GetNextToken(); // consume 'extern'
    return ParsePrototype();
}

// toplevelexpr ::= expression
auto static ParseTopLevelExpr() -> std::unique_ptr<AST::Function> {
    if (auto e = ParseExpression()) {
        auto proto = std::make_unique<AST::Prototype>("", std::vector<std::string>());
        return std::make_unique<AST::Function>(std::move(proto), std::move(e));
    }
    return nullptr;
}

auto static HandleDefinition() {
    if (ParseDefinition()) {
        fprintf(stderr, "parsed a function definition\n");
    } else {
        // Skip token for error recovery
        GetNextToken();
    }
}

auto static HandleExtern() {
    if (ParseExtern()) {
        fprintf(stderr, "parsed an extern\n");
    } else {
        // Skip token for error recovery
        GetNextToken();
    }
}

auto static HandleTopLevelExpression() {
    if (ParseTopLevelExpr()) {
        fprintf(stderr, "parsed a top-level expr\n");
    } else {
        // Skip token for error recovery
        GetNextToken();
    }
}

// top ::= definition | external | expression | ';'
auto MainLoop() -> void {
    fprintf(stderr, "ready> ");
    GetNextToken();
    auto prompt = false;
    
    while (true) {
        if (prompt) {
            prompt = false;
            fprintf(stderr, "ready> ");
        }
        
        auto ch = GetChar();
        auto token = GetTokenEnum();
        if (token == Token::kEof) {
            prompt = true;
        } else if (ch == ';') {
            GetNextToken();
        } else if (token == Token::kDef) {
            HandleDefinition();
        } else if (token == Token::kExtern) {
            HandleExtern();
        } else {
            HandleTopLevelExpression();
        }
    }
}
