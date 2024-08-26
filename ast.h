//
// Created by Cooper Roalson on 8/25/24.
//

#ifndef DESMOS_COMPILER_AST_H
#define DESMOS_COMPILER_AST_H

#include "compiler.h"
#include "frontend.h"

namespace AST {
    using namespace frontend;
    
    template <class T>
    using unq_ptr = std::unique_ptr<T>;

    struct ASTNode {
        SrcPos pos;

        virtual void semantic_analysis(Compiler* compiler, std::vector<Error> errors) = 0;
        virtual void compile(std::ostream& out) const = 0;

        explicit ASTNode(SrcPos pos) : pos(pos) {}
    };

    struct ExpressionNode : ASTNode {
        Type type;
        virtual int precedence() const = 0;
        explicit ExpressionNode(SrcPos pos) : ASTNode(pos) {}
    };

    struct LiteralNode : ExpressionNode {
        double value;

        int precedence() const override {return 0;}
        void semantic_analysis(Compiler* compiler, std::vector<Error> errors) override;
        void compile(std::ostream& out) const override;

        LiteralNode(SrcPos pos, Type type, double value) : ExpressionNode(pos), value(value) {
            this->type = type;
        }
    };

    struct IdentifierNode : ExpressionNode {
        std::string_view identifier;

        int precedence() const override {return 0;}
        void semantic_analysis(Compiler* compiler, std::vector<Error> errors) override;
        void compile(std::ostream& out) const override;

        IdentifierNode(SrcPos pos, std::string_view identifier) : ExpressionNode(pos), identifier(identifier) {}
    };

    struct DeclarationNode : ASTNode {
        Type type;
        std::string_view identifier;

        void semantic_analysis(Compiler* compiler, std::vector<Error> errors) override;
        void compile(std::ostream& out) const override;

        DeclarationNode(SrcPos pos, Type type, std::string_view identifier) : ASTNode(pos), type(type), identifier(identifier) {}
    };

    struct FunctionDeclarationNode : DeclarationNode {
        std::vector<unq_ptr<DeclarationNode>> parameters;

        void semantic_analysis(Compiler* compiler, std::vector<Error> errors) override;
        void compile(std::ostream& out) const override;

        FunctionDeclarationNode(SrcPos pos, Type type, std::string_view identifier) : DeclarationNode(pos, type, identifier), parameters() {}
    };

    struct BinaryOperatorNode : ExpressionNode {
        Operator op;
        unq_ptr<ExpressionNode> left;
        unq_ptr<ExpressionNode> right;

        int precedence() const override;
        void semantic_analysis(Compiler* compiler, std::vector<Error> errors) override;
        void compile(std::ostream& out) const override;

        BinaryOperatorNode(SrcPos pos, Operator op, unq_ptr<ExpressionNode> left, unq_ptr<ExpressionNode> right) : ExpressionNode(pos), op(op), left(std::move(left)), right(std::move(right)) {}
    };

    struct UnaryOperatorNode : ExpressionNode {
        Operator op;
        unq_ptr<ExpressionNode> expr;

        int precedence() const override;
        void semantic_analysis(Compiler* compiler, std::vector<Error> errors) override;
        void compile(std::ostream& out) const override;

        UnaryOperatorNode(SrcPos pos, Operator op, unq_ptr<ExpressionNode> expr) : ExpressionNode(pos), op(op), expr(std::move(expr)) {}
    };

    struct StatementNode : ASTNode {
        explicit StatementNode(SrcPos pos) : ASTNode(pos) {}
    };

    struct StatementBlockNode : StatementNode {
        std::vector<unq_ptr<StatementNode>> statements;

        void semantic_analysis(Compiler* compiler, std::vector<Error> errors) override;
        void compile(std::ostream& out) const override;

        explicit StatementBlockNode(SrcPos pos) : StatementNode(pos), statements() {}
    };

    struct InitializationStatementNode : StatementNode {
        unq_ptr<DeclarationNode> declaration;
        unq_ptr<ExpressionNode> value;

        void semantic_analysis(Compiler* compiler, std::vector<Error> errors) override;
        void compile(std::ostream& out) const override;

        InitializationStatementNode(unq_ptr<DeclarationNode> left, unq_ptr<ExpressionNode> right) : StatementNode(left->pos), declaration(std::move(left)), value(std::move(right)) {}
    };

    struct MainBlockNode : ASTNode {
        std::vector<unq_ptr<StatementNode>> statements;

        void semantic_analysis(Compiler* compiler, std::vector<Error> errors) override;
        void compile(std::ostream& out) const override;

        explicit MainBlockNode(SrcPos pos) : ASTNode(pos), statements() {}
    };
}

#endif //DESMOS_COMPILER_AST_H
