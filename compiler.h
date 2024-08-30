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

    bool isUnknown;
    bool isPrimitive;
    bool isConst;
    union {
        Primitive primitive;
        std::string_view identifier;
    } value;

    Type() : isUnknown(true), value{} {}
    Type(Primitive primitive, bool isConst = false) : isUnknown(false), isPrimitive(true), isConst(isConst), value{primitive} {}
    Type(std::string_view identifier, bool isConst = false) : isUnknown(false), isPrimitive(false), isConst(isConst), value{.identifier = identifier} {}

    bool matches(const Type& other) const {
        if (isUnknown) {return true;}
        if (isPrimitive) {return other.isPrimitive && value.primitive == other.value.primitive;}
        return !other.isPrimitive && value.identifier == other.value.identifier;
    }

    bool matches_primitive(Primitive primitive) const {
        return isUnknown || (isPrimitive && value.primitive == primitive);
    }

    std::string name() const {
        if (isUnknown) {return "unknown";}
        if (isPrimitive) {return PRIMITIVE_STRS[value.primitive];}
        return std::string(value.identifier);
    }
};

// Forward declarations
namespace AST {struct MainBlockNode; struct DeclarationNode;}
namespace frontend {class Parser;}

class SymbolScope {

    std::unordered_map<std::string_view, AST::DeclarationNode*> symbols;
    std::vector<SymbolScope> childScopes;
    SymbolScope* parentScope;
    std::string name;

public:
    SymbolScope() : symbols(), childScopes(), parentScope(nullptr), name() {}

    bool add_symbol(AST::DeclarationNode* declaration);
    SymbolScope* create_child_scope(std::string childName = "");
    SymbolScope* get_parent_scope() {return parentScope;}
    AST::DeclarationNode* find_symbol(std::string_view identifier);
};

class Compiler {
    friend bool compile_program(std::string&, std::ostream&);

    bool compile_frontend(std::string& source);
    void compile_backend(std::ostream& out);

public:
    std::unique_ptr<AST::MainBlockNode> ast;
    SymbolScope symbolTable;

    Compiler();

    static bool compile_program(std::string& source, std::ostream& out);
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
