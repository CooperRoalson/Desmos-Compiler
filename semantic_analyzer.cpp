//
// Created by Cooper Roalson on 8/25/24.
//

#include "frontend.h"
#include "ast.h"

namespace frontend {
    void semantic_analysis(Compiler* compiler, std::vector<Error>& errors) {
        compiler->ast->semantic_analysis(compiler, errors);
    }
}

using namespace AST;

void LiteralNode::semantic_analysis(Compiler* compiler, std::vector<Error> errors) {
}

void IdentifierNode::semantic_analysis(Compiler* compiler, std::vector<Error> errors) {
}

void DeclarationNode::semantic_analysis(Compiler* compiler, std::vector<Error> errors) {
}

void FunctionDeclarationNode::semantic_analysis(Compiler* compiler, std::vector<Error> errors) {
}

void BinaryOperatorNode::semantic_analysis(Compiler* compiler, std::vector<Error> errors) {
}

void UnaryOperatorNode::semantic_analysis(Compiler* compiler, std::vector<Error> errors) {
}

void StatementBlockNode::semantic_analysis(Compiler* compiler, std::vector<Error> errors) {
    for (auto& statement : statements) {
        statement->semantic_analysis(compiler, errors);
    }
}

void InitializationStatementNode::semantic_analysis(Compiler* compiler, std::vector<Error> errors) {

}

void MainBlockNode::semantic_analysis(Compiler* compiler, std::vector<Error> errors) {
    for (auto& statement : statements) {
        statement->semantic_analysis(compiler, errors);
    }
}


