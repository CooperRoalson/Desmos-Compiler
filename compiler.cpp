//
// Created by Cooper Roalson on 8/30/24.
//
#include "compiler.h"
#include "ast.h"

bool Compiler::compile_program(std::string& source, std::ostream& out) {
    Compiler compiler;
    if (!compiler.compile_frontend(source)) {return false;}
    compiler.compile_backend(out);
    return true;
}

Compiler::Compiler() : ast(), symbolTable() {}

SymbolScope* SymbolScope::create_child_scope(std::string childName) {
    SymbolScope& child = childScopes.emplace_back();
    child.parentScope = this;
    child.name = std::move(childName);
    return &child;
}

bool SymbolScope::add_symbol(AST::DeclarationNode* declaration) {
    if (symbols.contains(declaration->identifier)) {return false;}
    symbols[declaration->identifier] = declaration;
    return true;
}

AST::DeclarationNode* SymbolScope::find_symbol(std::string_view identifier) {
    if (symbols.contains(identifier)) {
        return symbols[identifier];
    } else if (parentScope) {
        return parentScope->find_symbol(identifier);
    } else {
        return nullptr;
    }
}
