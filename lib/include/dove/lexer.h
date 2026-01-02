#pragma once

#include "error.h"
#include "token.h"

#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>
#include <expected>

namespace Dove {

class Lexer {
private:
    const std::string_view source;
    std::vector<Token> tokens;
    std::optional<CompilerError> error;
    uint32_t cursor;
    uint32_t line;
    uint32_t column;

    std::expected<void, CompilerError> start();

    // Movement
    void advance();
    uint8_t peek();
    uint8_t peek_prev(uint32_t n = 1);
    uint8_t peek_next(uint32_t n = 1);
    void new_line();

    // Handlers
    std::expected<void, CompilerError> handle_identifier();
    std::expected<void, CompilerError> handle_string();
    std::expected<void, CompilerError> handle_character();
    std::expected<void, CompilerError> handle_number();
    void handle_comment();

    // Checker
    TokenType match_token_type(std::string_view str);

public:
    explicit Lexer(std::string_view source);

    std::expected<const std::vector<Token> *, CompilerError> get_tokens();
};

} // namespace Dove