//
// Created by Cooper Roalson on 7/12/24.
//

#include "frontend.h"
#include "compiler.h"

namespace frontend {

    static double read_num_literal(std::string& source, size_t start, int& len) {
        double result = 0;
        size_t i = start;
        while (i < source.size() && isdigit(source[i])) {
            result = result * 10 + (source[i] - '0');
            i++;
        }
        if (source[i] == '.') {
            i++;
            double factor = 0.1;
            while (i < source.size() && isdigit(source[i])) {
                result += (source[i] - '0') * factor;
                factor *= 0.1;
                i++;
            }
        }
        len = i - start;
        return result;
    }

    void lex(std::string& source, std::vector<Token>& tokens, std::vector<Error>& errors) {
        SrcPos pos = {0, 0, (size_t) -1};
        for (pos.i = 0; pos.i < source.size(); pos.i++) {
            char c = source[pos.i];
            char c2 = (pos.i + 1 == source.size()) ? (char) 0 : source[pos.i + 1];

            if (c == '/' && c2 == '/') {
                while (source[++pos.i] != '\n') {}
                c = source[pos.i];
            }

            pos.col++;
            if (c == '\n') {
                pos.line++;
                pos.col = -1;
                continue;
            }

            if (c == ' ' || c == '\t') {
                continue;
            }

            SrcPos start = pos;

            if (c == '(') {
                tokens.push_back({Token::LEFT_PAREN, start});
            } else if (c == ')') {
                tokens.push_back({Token::RIGHT_PAREN, start});
            } else if (c == '{') {
                tokens.push_back({Token::LEFT_BRACE, start});
            } else if (c == '}') {
                tokens.push_back({Token::RIGHT_BRACE, start});
            } else if (c == '[') {
                tokens.push_back({Token::LEFT_BRACKET, start});
            } else if (c == ']') {
                tokens.push_back({Token::RIGHT_BRACKET, start});
            } else if (c == ';') {
                tokens.push_back({Token::SEMICOLON, start});
            } else if (c == ',') {
                tokens.push_back({Token::COMMA, start});
            } else if (c == '$') {
                tokens.push_back({Token::DOLLAR, start});
            } else if (c == '.') {
                tokens.push_back({Token::DOT, start});
            } else if (c == '=' && c2 != '=') {
                tokens.push_back({Token::EQUALS, start});
            } else if (c == ':' && c2 == '=') {
                tokens.push_back({Token::ASSIGN, start});
                pos.i++;
                pos.col++;
            } else if (c == '+' && c2 == '=') {
                tokens.push_back({Token::PLUS_ASSIGN, start});
                pos.i++;
                pos.col++;
            } else if (c == '-' && c2 == '=') {
                tokens.push_back({Token::MINUS_ASSIGN, start});
                pos.i++;
                pos.col++;
            } else if (c == '*' && c2 == '=') {
                tokens.push_back({Token::MUL_ASSIGN, start});
                pos.i++;
                pos.col++;
            } else if (c == '/' && c2 == '=') {
                tokens.push_back({Token::DIV_ASSIGN, start});
                pos.i++;
                pos.col++;
            } else if (c == '%' && c2 == '=') {
                tokens.push_back({Token::MOD_ASSIGN, start});
                pos.i++;
                pos.col++;
            } else if (c == '+') {
                tokens.push_back({Token::PLUS, start});
            } else if (c == '-') {
                tokens.push_back({Token::MINUS, start});
            } else if (c == '*') {
                tokens.push_back({Token::MUL, start});
            } else if (c == '/') {
                tokens.push_back({Token::DIV, start});
            } else if (c == '%') {
                tokens.push_back({Token::MOD, start});
            } else if (c == '^') {
                tokens.push_back({Token::EXP, start});
            } else if (c == '&' && c2 == '&') {
                tokens.push_back({Token::AND, start});
                pos.i++;
                pos.col++;
            } else if (c == '|' && c2 == '|') {
                tokens.push_back({Token::OR, start});
                pos.i++;
                pos.col++;
            } else if (c == '?') {
                tokens.push_back({Token::QUESTION, start});
            } else if (c == ':') {
                tokens.push_back({Token::COLON, start});
            } else if (c == '|' && c2 != '|') {
                tokens.push_back({Token::ABS, start});
            } else if (c == '!' && c2 != '=') {
                tokens.push_back({Token::INVERT, start});
            } else if (c == '<' && c2 != '=') {
                tokens.push_back({Token::LESS, start});
            } else if (c == '>' && c2 != '=') {
                tokens.push_back({Token::GREATER, start});
            } else if (c == '<' && c2 == '=') {
                tokens.push_back({Token::LESS_EQUAL, start});
                pos.i++;
                pos.col++;
            } else if (c == '>' && c2 == '=') {
                tokens.push_back({Token::GREATER_EQUAL, start});
                pos.i++;
                pos.col++;
            } else if (c == '=' && c2 == '=') {
                tokens.push_back({Token::EQUAL_EQUAL, start});
                pos.i++;
                pos.col++;
            } else if (c == '!' && c2 == '=') {
                tokens.push_back({Token::NOT_EQUAL, start});
                pos.i++;
                pos.col++;
            } else if (pos.i + 4 < source.size() && std::string_view{source.data() + pos.i, 5} == "const") {
                tokens.push_back({Token::KW_CONST, start});
                pos.i += 4;
                pos.col += 4;
            } else if (pos.i + 5 < source.size() && std::string_view{source.data() + pos.i, 6} == "action") {
                tokens.push_back({Token::KW_ACTION, start});
                pos.i += 5;
                pos.col += 5;
            } else if (pos.i + 5 < source.size() && std::string_view{source.data() + pos.i, 6} == "struct") {
                tokens.push_back({Token::KW_STRUCT, start});
                pos.i += 5;
                pos.col += 5;
            } else if (pos.i + 3 < source.size() && std::string_view{source.data() + pos.i, 4} == "plot") {
                tokens.push_back({Token::KW_PLOT, start});
                pos.i += 3;
                pos.col += 3;
            } else if (pos.i + 5 < source.size() && std::string_view{source.data() + pos.i, 6} == "ticker") {
                tokens.push_back({Token::KW_TICKER, start});
                pos.i += 5;
                pos.col += 5;
            } else if (pos.i + 5 < source.size() && std::string_view{source.data() + pos.i, 6} == "hidden") {
                tokens.push_back({Token::KW_HIDDEN, start});
                pos.i += 5;
                pos.col += 5;
            } else if (pos.i + 3 < source.size() && std::string_view{source.data() + pos.i, 4} == "auto") {
                tokens.push_back({Token::KW_AUTO, start});
                pos.i += 3;
                pos.col += 3;
            } else if (pos.i + 1 < source.size() && std::string_view{source.data() + pos.i, 2} == "if") {
                tokens.push_back({Token::KW_IF, start});
                pos.i += 1;
                pos.col += 1;
            } else if (pos.i + 3 < source.size() && std::string_view{source.data() + pos.i, 4} == "else") {
                tokens.push_back({Token::KW_ELSE, start});
                pos.i += 3;
                pos.col += 3;
            } else if (pos.i + 3 < source.size() && std::string_view{source.data() + pos.i, 4} == "true") {
                tokens.push_back({Token::BOOL_LITERAL, start, .value.boolean = true});
                pos.i += 3;
                pos.col += 3;
            } else if (pos.i + 4 < source.size() && std::string_view{source.data() + pos.i, 5} == "false") {
                tokens.push_back({Token::BOOL_LITERAL, start, .value.boolean = false});
                pos.i += 4;
                pos.col += 4;
            } else {
                bool found = false;
                for (size_t i = 0; i < sizeof(Type::PRIMITIVE_STRS) / sizeof(Type::PRIMITIVE_STRS[0]); i++) {
                    auto& primitive = Type::PRIMITIVE_STRS[i];
                    auto len = strlen(primitive);
                    if (pos.i + (len - 1) >= source.size()) continue;
                    std::string_view slice(source.data() + pos.i, len);
                    if (slice == primitive) {
                        tokens.push_back({Token::PRIMITIVE, start, .value.uint = i});
                        pos.i += len - 1;
                        pos.col += len - 1;
                        found = true;
                        break;
                    }
                }
                if (found) continue;

                if (isdigit(c)) {
                    int len;
                    double num = read_num_literal(source, pos.i, len);
                    tokens.push_back({Token::NUM_LITERAL, start, .value.num = num});
                    pos.i += len - 1;
                    pos.col += len - 1;
                    bool f = false;
                    while (pos.i + 1 < source.size() && isalpha(source[pos.i + 1])) {
                        pos.i++;
                        pos.col++;
                        if (!f) {
                            f = true;
                            errors.push_back({pos, "Unexpected characters after number literal"});
                        }
                    }
                } else if (isalpha(c)) {
                    while (isalnum(c)) {
                        c = source[++pos.i];
                    }
                    std::string_view slice(source.data() + start.i, pos.i - start.i);
                    tokens.push_back({Token::IDENTIFIER, start, .value.string = slice});
                    pos.i--;
                    pos.col += pos.i - start.i;
                } else {
                    errors.push_back({pos, "Unexpected character '" + std::string(1, c) + "'"});
                }
            }
        }
        pos.col++;
        tokens.push_back({Token::FILE_END, pos});
    }
}
