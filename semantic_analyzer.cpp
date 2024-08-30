//
// Created by Cooper Roalson on 8/25/24.
//

#include "frontend.h"
#include "ast.h"

namespace frontend {
    void semantic_analysis(Compiler* compiler, std::vector<Error>& errors) {
        compiler->ast->postorder_traverse(compiler, errors, &AST::ASTNode::semantic_analysis);
    }

}

using namespace AST;

void IdentifierNode::semantic_analysis(Compiler* compiler, std::vector<Error>& errors) {
    DeclarationNode* declaration = scope->find_symbol(identifier);
    if (!declaration) {
        errors.emplace_back(pos, "Symbol not found in current scope: '" + std::string(identifier) + "'");
    } else {
        type = declaration->type;
    }
}

void BinaryOperatorNode::semantic_analysis(Compiler* compiler, std::vector<Error>& errors) {
    bool isValid = false;
    switch (op) {
        case Operator::PLUS:
        case Operator::MINUS:
            if ((left->type.matches_primitive(Type::NUM) && right->type.matches_primitive(Type::NUM))
                || (left->type.matches_primitive(Type::POINT) && right->type.matches_primitive(Type::POINT))) {
                    isValid = true;
                    type = {Type::NUM};
            }
            break;
        case Operator::MUL:
            if (left->type.matches_primitive(Type::NUM) && right->type.matches_primitive(Type::POINT)) {
                isValid = true;
                type = {Type::POINT};
            }
        case Operator::DIV:
            if (left->type.matches_primitive(Type::NUM) && right->type.matches_primitive(Type::NUM)) {
                isValid = true;
                type = {Type::NUM};
            } else if (left->type.matches_primitive(Type::POINT) && right->type.matches_primitive(Type::NUM)) {
                isValid = true;
                type = {Type::POINT};
            }
            break;
        case Operator::MOD:
        case Operator::EXP:
            if (left->type.matches_primitive(Type::NUM) || right->type.matches_primitive(Type::NUM)) {
                isValid = true;
                type = {Type::NUM};
            }
            break;
        case Operator::AND:
        case Operator::OR:
            if (left->type.matches_primitive(Type::BOOL) && right->type.matches_primitive(Type::BOOL)) {
                isValid = true;
                type = {Type::BOOL};
            }
            break;
        default:
            throw std::runtime_error("Invalid binary operator: " + std::to_string(op));
    }
    if (!isValid) {
        errors.emplace_back(pos, "Operator cannot be applied to operands of type '" + left->type.name() + "' and '" + right->type.name() + "'");
    }
}

void UnaryOperatorNode::semantic_analysis(Compiler* compiler, std::vector<Error>& errors) {
    bool isValid = false;
    switch (op) {
        case Operator::MINUS:
            if (expr->type.matches_primitive(Type::NUM)) {
                isValid = true;
                type = {Type::NUM};
            } else if (expr->type.matches_primitive(Type::POINT)) {
                isValid = true;
                type = {Type::POINT};
            }
            break;
        case Operator::INVERT:
            if (expr->type.matches_primitive(Type::BOOL)) {
                isValid = true;
                type = {Type::BOOL};
            }
            break;
        default:
            throw std::runtime_error("Invalid unary operator: " + std::to_string(op));
    }
    if (!isValid) {
        errors.emplace_back(pos, "Operator cannot be applied to operand of type '" + expr->type.name() + "'");
    }
}
