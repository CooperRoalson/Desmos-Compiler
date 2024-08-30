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

        virtual void postorder_traverse(Compiler* compiler, std::vector<Error>& errors,
                                             void (ASTNode::*func)(Compiler*, std::vector<Error>&));
        virtual void semantic_analysis(Compiler* compiler, std::vector<Error>& errors) {}
        virtual void compile(std::ostream& out) const = 0;

        explicit ASTNode(SrcPos pos) : pos(pos) {}
        virtual ~ASTNode() = default;
    };

    struct ExpressionNode : ASTNode {
        Type type;
        virtual int precedence() const = 0;
        explicit ExpressionNode(SrcPos pos) : ASTNode(pos) {}
    };

    struct LiteralNode : ExpressionNode {
        double value;

        int precedence() const override {return 0;}
        void compile(std::ostream& out) const override;

        LiteralNode(SrcPos pos, Type type, double value) : ExpressionNode(pos), value(value) {
            this->type = type;
        }
    };

    struct IdentifierNode : ExpressionNode {
        std::string_view identifier;
        SymbolScope* scope;

        int precedence() const override {return 0;}
        void semantic_analysis(Compiler* compiler, std::vector<Error>& errors) override;
        void compile(std::ostream& out) const override;

        IdentifierNode(SrcPos pos, std::string_view identifier, SymbolScope* scope) : ExpressionNode(pos), identifier(identifier), scope(scope) {}
    };

    struct DeclarationNode : ASTNode {
        Type type;
        std::string_view identifier;
        SymbolScope* scope;

        virtual bool isFunction() const {return false;}
        void compile(std::ostream& out) const override;

        DeclarationNode(SrcPos pos, Type type, std::string_view identifier, SymbolScope* scope) : ASTNode(pos), type(type), identifier(identifier), scope(scope) {}
    };

    struct FunctionDeclarationNode : DeclarationNode {
        std::vector<unq_ptr<DeclarationNode>> parameters;

        bool isFunction() const override {return true;}
        void postorder_traverse(Compiler* compiler, std::vector<Error>& errors,
                                     void (ASTNode::*func)(Compiler*, std::vector<Error>&)) override;
        void compile(std::ostream& out) const override;

        FunctionDeclarationNode(SrcPos pos, Type type, std::string_view identifier, SymbolScope* scope) : DeclarationNode(pos, type, identifier, scope), parameters() {}
    };

    struct BinaryOperatorNode : ExpressionNode {
        Operator op;
        unq_ptr<ExpressionNode> left;
        unq_ptr<ExpressionNode> right;

        int precedence() const override;
        void postorder_traverse(Compiler* compiler, std::vector<Error>& errors,
                                     void (ASTNode::*func)(Compiler*, std::vector<Error>&)) override;
        void semantic_analysis(Compiler* compiler, std::vector<Error>& errors) override;
        void compile(std::ostream& out) const override;

        BinaryOperatorNode(SrcPos pos, Operator op, unq_ptr<ExpressionNode> left, unq_ptr<ExpressionNode> right) : ExpressionNode(pos), op(op), left(std::move(left)), right(std::move(right)) {}
    };

    struct UnaryOperatorNode : ExpressionNode {
        Operator op;
        unq_ptr<ExpressionNode> expr;

        int precedence() const override;
        void postorder_traverse(Compiler* compiler, std::vector<Error>& errors,
                                     void (ASTNode::*func)(Compiler*, std::vector<Error>&)) override;
        void semantic_analysis(Compiler* compiler, std::vector<Error>& errors) override;
        void compile(std::ostream& out) const override;

        UnaryOperatorNode(SrcPos pos, Operator op, unq_ptr<ExpressionNode> expr) : ExpressionNode(pos), op(op), expr(std::move(expr)) {}
    };

    struct StatementNode : ASTNode {
        explicit StatementNode(SrcPos pos) : ASTNode(pos) {}
    };

    struct StatementBlockNode : StatementNode {
        std::vector<unq_ptr<StatementNode>> statements;

        void postorder_traverse(Compiler* compiler, std::vector<Error>& errors,
                                     void (ASTNode::*func)(Compiler*, std::vector<Error>&)) override;
        void compile(std::ostream& out) const override;

        explicit StatementBlockNode(SrcPos pos) : StatementNode(pos), statements() {}
    };

    struct InitializationStatementNode : StatementNode {
        unq_ptr<DeclarationNode> declaration;
        unq_ptr<ExpressionNode> value;

        void postorder_traverse(Compiler* compiler, std::vector<Error>& errors,
                                     void (ASTNode::*func)(Compiler*, std::vector<Error>&)) override;
        void compile(std::ostream& out) const override;

        InitializationStatementNode(unq_ptr<DeclarationNode> left, unq_ptr<ExpressionNode> right) : StatementNode(left->pos), declaration(std::move(left)), value(std::move(right)) {}
    };

    struct MainBlockNode : ASTNode {
        std::vector<unq_ptr<StatementNode>> statements;

        void postorder_traverse(Compiler* compiler, std::vector<Error>& errors,
                                     void (ASTNode::*func)(Compiler*, std::vector<Error>&)) override;
        void compile(std::ostream& out) const override;

        explicit MainBlockNode(SrcPos pos) : ASTNode(pos), statements() {}
    };
}

#endif //DESMOS_COMPILER_AST_H
