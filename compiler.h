//
// Created by Cooper Roalson on 7/27/24.
//

#ifndef DESMOS_COMPILER_COMPILER_H
#define DESMOS_COMPILER_COMPILER_H

#include <string>

struct Type {
    static constexpr const char* PRIMITIVE_STRS[5] = {"num", "point", "bool", "color", "polygon"};
    enum Primitive {
        NUM,
        POINT,
        BOOL,
        COLOR,
        POLYGON
    };

    bool isUndefined;
    bool isPrimitive;
    bool isConst;
    union {
        Primitive primitive;
        std::string_view identifier;
    } value;

    Type() : isUndefined(true), value{} {}
    Type(Primitive primitive, bool isConst) : isUndefined(false), isPrimitive(true), isConst(isConst), value{primitive} {}
    Type(std::string_view identifier, bool isConst) : isUndefined(false), isPrimitive(false), isConst(isConst), value{.identifier = identifier} {}

};

class SymbolTable {
    struct SymbolTableNode {
        Type type;
    };

    std::unordered_map<std::string, SymbolTableNode> symbols;
    std::vector<SymbolTable> childScopes;

public:
    SymbolTable() : symbols(), childScopes() {}

};

// Forward declarations
namespace AST {class MainBlockNode;}
namespace frontend {class Parser;}

static bool compile_program(std::string& source, std::ostream& out);

class Compiler {
    friend bool compile_program(std::string&, std::ostream&);

    bool compile_frontend(std::string& source);
    void compile_backend(std::ostream& out);

public:
    std::unique_ptr<AST::MainBlockNode> ast;
    SymbolTable symbolTable;

    Compiler();
};

// Order of operations:
//  0. (expr), |expr|, lit, ident
//  1. func(), arr[], foo.bar (left)
//  2. x^y (right)
//  3. -x, !x (right)
//  4. x*y, x/y, x%y (left)
//  5. x+y, x-y (left)
//  6. x<y, x<=y, x>y, x>=y (left)
//  7. x==y, x!=y (left)
//  8. x&&y (left)
//  9. x||y (left)
// 10. x?b:c (right)

enum Operator {
    EQUALS, ASSIGN, PLUS_ASSIGN, MINUS_ASSIGN, MUL_ASSIGN, DIV_ASSIGN, MOD_ASSIGN,
    PLUS, MINUS, MUL, DIV, MOD, EXP, ABS, INVERT, AND, OR,

    LESS, GREATER, LESS_EQUAL, GREATER_EQUAL, EQUAL_EQUAL, NOT_EQUAL
};

#endif //DESMOS_COMPILER_COMPILER_H
