//
// Created by Cooper Roalson on 7/10/24.
//

#include <cstdio>
#include <iostream>

#include "frontend.h"
#include "ast.h"

namespace frontend {

    static void print_errors(std::vector<Error>& errors, std::string& source) {
        std::cerr << errors.size() << " errors found during compilation:\n\n";
        for (auto& error: errors) {
            long begin = error.pos.i;
            while (begin-1 >= 0 && source[begin] != '\n') {begin--;}
            if (source[begin] == '\n') begin++;
            long end = error.pos.i;
            while (end+1 < source.size() && source[end] != '\n') {end++;}
            if (source[end] == '\n') end--;

            fprintf(stderr, "Error at line %lu, col %lu: %s\n", error.pos.line + 1, error.pos.col + 1, error.message.c_str());
            std::cerr << "   " << std::string_view (source.data() + begin, end - begin + 1) << std::endl;
            std::cerr << std::string(3 + error.pos.i - begin, ' ') << '^' << std::endl << std::endl;
        }
        exit(1);
    }

}

using namespace frontend;
bool Compiler::compile_frontend(std::string& source) {
    std::vector<Error> errors;

    std::vector<Token> tokens;
    lex(source, tokens, errors);
    if (!errors.empty()) {
        print_errors(errors, source);
        return false;
    }

    parse(this, tokens, errors);
    if (!errors.empty()) {
        print_errors(errors, source);
        return false;
    }

    semantic_analysis(this, errors);
    if (!errors.empty()) {
        print_errors(errors, source);
        return false;
    }

    return true;
}
