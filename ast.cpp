//
// Created by Cooper Roalson on 8/30/24.
//
#include "ast.h"

using namespace AST;

void FunctionDeclarationNode::postorder_traverse(Compiler* compiler, std::vector<Error>& errors,
                                                 void (ASTNode::*func)(Compiler*, std::vector<Error>&)) {
    for (auto& param : parameters) {
        param->postorder_traverse(compiler, errors, func);
    }
    DeclarationNode::postorder_traverse(compiler, errors, func);
}

void BinaryOperatorNode::postorder_traverse(Compiler* compiler, std::vector<Error>& errors,
                                            void (ASTNode::*func)(Compiler*, std::vector<Error>&)) {
    left->postorder_traverse(compiler, errors, func);
    right->postorder_traverse(compiler, errors, func);
    ExpressionNode::postorder_traverse(compiler, errors, func);
}

void UnaryOperatorNode::postorder_traverse(Compiler* compiler, std::vector<Error>& errors,
                                           void (ASTNode::*func)(Compiler*, std::vector<Error>&)) {
    expr->postorder_traverse(compiler, errors, func);
    ExpressionNode::postorder_traverse(compiler, errors, func);
}

void StatementBlockNode::postorder_traverse(Compiler* compiler, std::vector<Error>& errors,
                                            void (ASTNode::*func)(Compiler*, std::vector<Error>&)) {
    for (auto& statement : statements) {
        statement->postorder_traverse(compiler, errors, func);
    }
    StatementNode::postorder_traverse(compiler, errors, func);
}

void InitializationStatementNode::postorder_traverse(Compiler* compiler, std::vector<Error>& errors,
                                                     void (ASTNode::*func)(Compiler*, std::vector<Error>&)) {
    declaration->postorder_traverse(compiler, errors, func);
    value->postorder_traverse(compiler, errors, func);
    StatementNode::postorder_traverse(compiler, errors, func);
}

void MainBlockNode::postorder_traverse(Compiler* compiler, std::vector<Error>& errors,
                                       void (ASTNode::*func)(Compiler*, std::vector<Error>&)) {
    for (auto& statement : statements) {
        statement->postorder_traverse(compiler, errors, func);
    }
    ASTNode::postorder_traverse(compiler, errors, func);
}

void ASTNode::postorder_traverse(Compiler* compiler, std::vector<Error>& errors,
                                 void (ASTNode::*func)(Compiler*, std::vector<Error>&)) {
    (this->*func)(compiler, errors);
}
