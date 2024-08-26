//
// Created by Cooper Roalson on 7/12/24.
//

#include <memory>
#include <sstream>

#include "compiler.h"
#include "frontend.h"
#include "ast.h"

namespace frontend {
    using namespace AST;

    template <class T>
    using unq_ptr = std::unique_ptr<T>;

    class Parser {
        Compiler* compiler;
        const std::vector<Token>& tokens;
        std::vector<Error>& errors;
        long i;

        bool accept_token(Token::Type token, bool required = false);

        unq_ptr<ExpressionNode> parse_binary_operator(Token::Type opTypes[4], bool leftAssoc, unq_ptr<ExpressionNode> (Parser::*parse_next)());
        unq_ptr<ExpressionNode> parse_e0();
        unq_ptr<ExpressionNode> parse_e1();
        unq_ptr<ExpressionNode> parse_e2();
        unq_ptr<ExpressionNode> parse_e3();
        unq_ptr<ExpressionNode> parse_e4();
        unq_ptr<ExpressionNode> parse_e5();
        unq_ptr<ExpressionNode> parse_e6();
        unq_ptr<ExpressionNode> parse_e7();
        unq_ptr<ExpressionNode> parse_e8();
        unq_ptr<ExpressionNode> parse_e9();
        unq_ptr<ExpressionNode> parse_e10();
        unq_ptr<ExpressionNode> parse_expression(bool required = false);

        Type parse_type(bool required = false);
        unq_ptr<DeclarationNode> parse_declaration(bool required = false);
        unq_ptr<InitializationStatementNode> parse_initialization_statement(bool required = false);
        unq_ptr<StatementNode> parse_statement(bool required = false);
        unq_ptr<StatementBlockNode> parse_statement_block(bool required = false);
        unq_ptr<MainBlockNode> parse_main_block();

    public:
        Parser(Compiler* compiler, const std::vector<Token>& tokens, std::vector<Error>& errors) : compiler(compiler), tokens(tokens), errors(errors), i(0) {}
        void parse();
    };

    void parse(Compiler* compiler, const std::vector<Token>& tokens, std::vector<Error>& errors) {
        Parser parser(compiler, tokens, errors);
        parser.parse();
    }
    void Parser::parse() {
        compiler->ast = parse_main_block();
    }


    bool Parser::accept_token(Token::Type token, bool required) {
        if (i < tokens.size() && tokens[i].type == token) {
            i++;
            return true;
        }
        if (required) {
            std::stringstream message;
            if (i + 1 < tokens.size() && tokens[i + 1].type == token) {
                message << "Unexpected token: " << tokens[i].name();
                errors.push_back({tokens[i].pos, message.str()});
                i += 2;
                return true;
            }
            message << "Expected token: " << token;
            if (i < tokens.size()) {message << ", got: " << tokens[i].name();}
            errors.push_back({tokens[i].pos, message.str()});
        }
        return false;
    }

    Type Parser::parse_type(bool required) {
        long start = i;
        bool isConst = accept_token(Token::KW_CONST);

        if (accept_token(Token::PRIMITIVE)) {
            return {(Type::Primitive) tokens[i - 1].value.uint, isConst};
        } else if (accept_token(Token::IDENTIFIER)) {
            return {tokens[i - 1].value.string, isConst};
        }

        if (required) {
            errors.push_back({tokens[start].pos, "Expected type"});
        } else {
            i = start;
        }
        return {};
    }

    unq_ptr<DeclarationNode> Parser::parse_declaration(bool required) {
        long start = i;
        Type type = parse_type();
        if (type.isUndefined) {
            if (required) {errors.push_back({tokens[start].pos, "Expected declaration"});}
            return nullptr;
        }

        if (!accept_token(Token::IDENTIFIER)) {
            if (required) {errors.push_back({tokens[start].pos, "Expected declaration"});}
            i = start;
            return nullptr;
        }

        // TODO: check for parameters

        return std::make_unique<DeclarationNode>(tokens[start].pos, type, tokens[i - 1].value.string);
    }

    static Operator get_operator(Token::Type type) {
        switch (type) {
            case Token::EQUALS: return Operator::EQUALS;
            case Token::ASSIGN: return Operator::ASSIGN;
            case Token::PLUS_ASSIGN: return Operator::PLUS_ASSIGN;
            case Token::MINUS_ASSIGN: return Operator::MINUS_ASSIGN;
            case Token::MUL_ASSIGN: return Operator::MUL_ASSIGN;
            case Token::DIV_ASSIGN: return Operator::DIV_ASSIGN;
            case Token::MOD_ASSIGN: return Operator::MOD_ASSIGN;
            case Token::PLUS: return Operator::PLUS;
            case Token::MINUS: return Operator::MINUS;
            case Token::MUL: return Operator::MUL;
            case Token::DIV: return Operator::DIV;
            case Token::MOD: return Operator::MOD;
            case Token::EXP: return Operator::EXP;
            case Token::ABS: return Operator::ABS;
            case Token::INVERT: return Operator::INVERT;
            case Token::AND: return Operator::AND;
            case Token::OR: return Operator::OR;
            case Token::LESS: return Operator::LESS;
            case Token::GREATER: return Operator::GREATER;
            case Token::LESS_EQUAL: return Operator::LESS_EQUAL;
            case Token::GREATER_EQUAL: return Operator::GREATER_EQUAL;
            case Token::EQUAL_EQUAL: return Operator::EQUAL_EQUAL;
            case Token::NOT_EQUAL: return Operator::NOT_EQUAL;
            default:
                std::stringstream ss;
                ss << "Token is not an operator: " << Token::NAMES[type];
                throw std::runtime_error(ss.str());
        }
    }

    unq_ptr<ExpressionNode> Parser::parse_binary_operator(Token::Type opTypes[4], bool leftAssoc, unq_ptr<ExpressionNode> (Parser::*parse_next)()) {
        unq_ptr<ExpressionNode> node = (this->*parse_next)();
        if (!node) {return nullptr;}

        ExpressionNode* current = node.get();
        bool isBinop = false;
        while (accept_token(opTypes[0]) || accept_token(opTypes[1]) || accept_token(opTypes[2]) || accept_token(opTypes[3])) {
            Token op = tokens[i - 1];
            unq_ptr<ExpressionNode> right = (this->*parse_next)();
            if (!right) {
                errors.push_back({tokens[i].pos, "Expected expression"});
                break;
            }

            if (!leftAssoc && isBinop) {
                auto binopNode = (BinaryOperatorNode*) current;
                binopNode->right = std::make_unique<BinaryOperatorNode>(op.pos, get_operator(op.type), std::move(binopNode->right), std::move(right));
                current = binopNode->right.get();
            } else {
                node = std::make_unique<BinaryOperatorNode>(op.pos, get_operator(op.type), std::move(node), std::move(right));
                current = node.get();
            }
            isBinop = true;
        }
        return node;
    }

    unq_ptr<ExpressionNode> Parser::parse_e0() {
        // (expr), literals, identifiers
        if (accept_token(Token::LEFT_PAREN)) {
            unq_ptr<ExpressionNode> node = parse_expression(true);
            accept_token(Token::RIGHT_PAREN, true);
            return node;
        } else if (accept_token(Token::IDENTIFIER)) {
            return std::make_unique<IdentifierNode>(tokens[i - 1].pos, tokens[i - 1].value.string);
        } else if (accept_token(Token::NUM_LITERAL)) {
            return std::make_unique<LiteralNode>(tokens[i - 1].pos, Type(Type::NUM, true), tokens[i - 1].value.num);
        } else if (accept_token(Token::BOOL_LITERAL)) {
            return std::make_unique<LiteralNode>(tokens[i - 1].pos, Type(Type::BOOL, true), tokens[i - 1].value.boolean ? 1 : 0);
        } else {
            return nullptr;
        }
    }

    unq_ptr<ExpressionNode> Parser::parse_e1() {
        // func(), arr[], foo.bar
        return parse_e0();
    }

    unq_ptr<ExpressionNode> Parser::parse_e2() {
        // ^
        Token::Type ops[4] = {Token::EXP, Token::EXP, Token::EXP, Token::EXP};
        return parse_binary_operator(ops, false, &Parser::parse_e1);
    }

    unq_ptr<ExpressionNode> Parser::parse_e3() {
        // -, !
        unq_ptr<ExpressionNode> node = nullptr;
        UnaryOperatorNode* currentOp = nullptr;
        while (accept_token(Token::MINUS) || accept_token(Token::INVERT)) {
            Token op = tokens[i - 1];
            if (currentOp) {
                currentOp->expr = std::make_unique<UnaryOperatorNode>(op.pos, get_operator(op.type), nullptr);
                currentOp = (UnaryOperatorNode*) (currentOp->expr.get());
            } else {
                node = std::make_unique<UnaryOperatorNode>(op.pos, get_operator(op.type), nullptr);
                currentOp = (UnaryOperatorNode*) node.get();
            }
        }

        unq_ptr<ExpressionNode> expr = parse_e2();
        if (!expr) {return nullptr;}

        if (currentOp) {
            currentOp->expr = std::move(expr);
        } else {
            node = std::move(expr);
        }
        return node;
    }

    unq_ptr<ExpressionNode> Parser::parse_e4() {
        // +, -
        Token::Type ops[4] = {Token::MUL, Token::DIV, Token::MOD, Token::MOD};
        return parse_binary_operator(ops, true, &Parser::parse_e3);
    }

    unq_ptr<ExpressionNode> Parser::parse_e5() {
        // +, -
        Token::Type ops[4] = {Token::PLUS, Token::MINUS, Token::MINUS, Token::MINUS};
        return parse_binary_operator(ops, true, &Parser::parse_e4);
    }

    unq_ptr<ExpressionNode> Parser::parse_e6() {
        // <, >, <=, >=
        Token::Type ops[4] = {Token::LESS, Token::GREATER, Token::LESS_EQUAL, Token::GREATER_EQUAL};
        return parse_binary_operator(ops, true, &Parser::parse_e5);
    }

    unq_ptr<ExpressionNode> Parser::parse_e7() {
        // ==, !=
        Token::Type ops[4] = {Token::EQUAL_EQUAL, Token::NOT_EQUAL, Token::NOT_EQUAL, Token::NOT_EQUAL};
        return parse_binary_operator(ops, true, &Parser::parse_e6);
    }

    unq_ptr<ExpressionNode> Parser::parse_e8() {
        // &&
        Token::Type ops[4] = {Token::AND, Token::AND, Token::AND, Token::AND};
        return parse_binary_operator(ops, true, &Parser::parse_e7);
    }

    unq_ptr<ExpressionNode> Parser::parse_e9() {
        // ||
        Token::Type ops[4] = {Token::OR, Token::OR, Token::OR, Token::OR};
        return parse_binary_operator(ops, true, &Parser::parse_e8);
    }

    unq_ptr<ExpressionNode> Parser::parse_e10() {
        // ?:
        return parse_e9();
    }

    unq_ptr<ExpressionNode> Parser::parse_expression(bool required) {
        unq_ptr<ExpressionNode> node = parse_e10();
        if (!node && required) {
            errors.push_back({tokens[i].pos, "Expected expression"});
        }
        return node;
    }

    unq_ptr<InitializationStatementNode> Parser::parse_initialization_statement(bool required) {
        long start = i;
        unq_ptr<DeclarationNode> declaration = parse_declaration();
        if (!declaration) {
            if (required) {errors.push_back({tokens[start].pos, "Expected equals statement"});}
            return nullptr;
        }

        accept_token(Token::EQUALS, true);
//        if (!accept_token(Token::EQUALS)) {
//            if (required) {errors.push_back({tokens[start].pos, "Expected equals statement"});}
//            i = start;
//            return nullptr;
//        }

        unq_ptr<ExpressionNode> value = parse_expression(true);

        accept_token(Token::SEMICOLON, true);
        return std::make_unique<InitializationStatementNode>(std::move(declaration), std::move(value));
    }

    unq_ptr<StatementNode> Parser::parse_statement(bool required) {
        unq_ptr<StatementNode> node;

        if ((node = parse_statement_block())) {
            return node;
        } else if ((node = parse_initialization_statement())) {
            return node;
        }

        if (required) {
            errors.push_back({tokens[i].pos, "Expected statement"});
        }
        return nullptr;
    }

    unq_ptr<StatementBlockNode> Parser::parse_statement_block(bool required) {
        if (!accept_token(Token::LEFT_BRACE, required)) {
            return nullptr;
        }

        unq_ptr<StatementBlockNode> node = std::make_unique<StatementBlockNode>(tokens[i - 1].pos);

        bool flag = true;
        while (tokens[i].type != Token::RIGHT_BRACE && tokens[i].type != Token::FILE_END) {
            unq_ptr<StatementNode> statement = parse_statement(flag);
            if (!statement) {
                flag = false;
                i++;
            } else {
                flag = true;
                node->statements.push_back(std::move(statement));
            }
        }
        accept_token(Token::RIGHT_BRACE, true);

        return node;
    }

    unq_ptr<MainBlockNode> Parser::parse_main_block() {
        unq_ptr<MainBlockNode> node = std::make_unique<MainBlockNode>(tokens[i].pos);

        bool flag = true;
        while (tokens[i].type != Token::FILE_END) {
            unq_ptr<StatementNode> statement = parse_statement(flag);
            if (!statement) {
                // If the next statement is invalid, skip until it isn't
                flag = false;
                i++;
            } else {
                flag = true;
                node->statements.push_back(std::move(statement));
            }

        }
        accept_token(Token::FILE_END, true);

        return node;
    }

    
}
