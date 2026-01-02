#include "dove/dove.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <print>

bool read_file(const std::string &filename, std::string *out);

int main() {
    std::string file("examples/exp1.dv");
    std::string src;

    if (!read_file(file, &src)) {
        std::cout << "Error reading file" << std::endl;
        return 1;
    }

    Dove::Lexer lexer(src);
    auto res = lexer.get_tokens();
    if (!res) {
        std::println("{}", res.error().format());
        return 1;
    }

    const std::vector<Dove::Token> &tokens = *res.value();
    for (auto t : tokens) {
        std::println("[T{:03d}] {:02d}:{:02d}: {}", static_cast<uint8_t>(t.type), t.line, t.column, t.str);
    }
    return 0;
}

bool read_file(const std::string &filename, std::string *out) {
    if (!out) {
        return false;
    }

    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    *out = buffer.str();

    return true;
}