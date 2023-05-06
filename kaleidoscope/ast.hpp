//
//  ast.hpp
//  kaleidoscope
//
//  Created by Alexander Balaban on 06/05/2023.
//

#ifndef ast_hpp
#define ast_hpp

#include <string>

namespace AST {

// Expr – Base class for all expression nodes
class Expr {
public:
    virtual ~Expr() = default;
};

// NumberExpr – Expression class for numer literals
class NumberExpr: public Expr {
    double val_;
    
public:
    NumberExpr(double val): val_(val) {}
};

// VariableExpr – Expression class for referencing a variable
class VariableExpr: public Expr {
    std::string name_;
    
public:
    VariableExpr(const std::string &name): name_(name) {}
};

// BinaryExpr – Expression class for a binary operator
class BinaryExpr: public Expr {
    char op_;
    std::unique_ptr<Expr> lhs_, rhs_;
    
public:
    BinaryExpr(char op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs)
    : op_(op), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}
};

// CallExpr – Expression class for function calls
class CallExpr: public Expr {
    std::string callee_;
    std::vector<std::unique_ptr<Expr>> args_;
    
public:
    CallExpr(const std::string &callee, std::vector<std::unique_ptr<Expr>> args)
    : callee_(callee), args_(std::move(args)) {}
};


// PrototypeAST - This class represents the "prototype" for a function,
// which captures its name, and its argument names (thus implicitly the number
// of arguments the function takes).
class Prototype {
    std::string name_;
    std::vector<std::string> args_;
    
public:
    Prototype(const std::string &name, std::vector<std::string> args)
    : name_(name), args_(args) {}
};

// FunctionAST - This class represents a function definition itself
class Function {
    std::unique_ptr<Prototype> proto_;
    std::unique_ptr<Expr> body_;
    
public:
    Function(std::unique_ptr<Prototype> proto, std::unique_ptr<Expr> body)
    : proto_(std::move(proto)), body_(std::move(body)) {{}}
};

}
#endif /* ast_hpp */

