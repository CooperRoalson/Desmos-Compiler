//
// Created by Cooper Roalson on 7/11/24.
//

#ifndef DESMOS_COMPILER_FRONTEND_H
#define DESMOS_COMPILER_FRONTEND_H

#include <vector>
#include <string>

#include "compiler.h"

// Forward declaration
namespace AST {struct MainBlockNode;}

namespace frontend {

    struct SrcPos {
        size_t i, line, col;
    };

    struct Token {
        enum Type {
            FILE_END,

            NUM_LITERAL, BOOL_LITERAL, IDENTIFIER, PRIMITIVE,

            KW_CONST, KW_ACTION, KW_STRUCT, KW_PLOT, KW_TICKER, KW_HIDDEN, KW_AUTO, KW_IF, KW_ELSE,

            LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, LEFT_BRACKET, RIGHT_BRACKET,

            SEMICOLON, COMMA, DOLLAR, DOT, QUESTION, COLON,

            EQUALS, ASSIGN, PLUS_ASSIGN, MINUS_ASSIGN, MUL_ASSIGN, DIV_ASSIGN, MOD_ASSIGN,
            PLUS, MINUS, MUL, DIV, MOD, EXP, ABS, INVERT, AND, OR,

            LESS, GREATER, LESS_EQUAL, GREATER_EQUAL, EQUAL_EQUAL, NOT_EQUAL
        } type;
        SrcPos pos;
        union {
            double num;
            size_t uint;
            bool boolean;
            std::string_view string;
        } value;

        static constexpr const char* NAMES[49] = {
                "end of file", "number literal", "boolean literal", "identifier", "primitive",
                "'const'", "'action'", "'struct'", "'plot'", "'ticker'", "'hidden'", "'auto'", "'if'", "'else'",
                "'('", "')'", "'{'", "'}'", "'['", "']'", "';'", "','", "'$'", "'.'", "'?'", "':'",
                "'='", "':='", "'+='", "'-='", "'*='", "'/='", "'%='",
                "'+'", "'-'", "'*'", "'/'", "'%'", "'^'", "'|'", "'!'", "'&&'", "'||'",
                "'<'", "'>'", "'<='", "'>='", "'=='", "'!='"
        };
        const char* name() const {return NAMES[type];}
    };

    std::ostream& operator<<(std::ostream& out, const Token::Type& tokenType);

    struct Error {
        SrcPos pos;
        std::string message;
    };

    void lex(std::string& source, std::vector<Token>& tokens, std::vector<Error>& errors);
    void parse(Compiler* compiler, const std::vector<Token>& tokens, std::vector<Error>& errors);
    void semantic_analysis(Compiler* compiler, std::vector<Error>& errors);
}




#endif //DESMOS_COMPILER_FRONTEND_H
