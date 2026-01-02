#pragma once

#include <cstdint>
#include <format>
#include <string>
#include <expected>
#include <variant>

namespace Dove {

// TODO: Redo error handling

enum class LexerError {
    UnexpectedLexeme,
    StringNotTerminated,
    EmptyCharacterLiteral,
    ExpectedCharNotString,
};

enum class ParserError {};

using ErrorType = std::variant<LexerError, ParserError>;

class CompilerError {
private:
    ErrorType type;
    uint32_t line;
    uint32_t column;
    std::string message;

    uint16_t error_code() const {
        return std::visit(
            [](auto &&err) -> uint16_t {
                using T = std::decay_t<decltype(err)>;
                constexpr uint16_t phase_offset = std::is_same_v<T, LexerError>    ? 1000
                                                  : std::is_same_v<T, ParserError> ? 2000
                                                                                   : 9000;
                return phase_offset + static_cast<uint16_t>(err);
            },
            type);
    }

public:
    CompilerError(ErrorType type, uint32_t line, uint32_t column, std::string message)
        : type(type), line(line), column(column), message(std::move(message)) {}

    std::unexpected<CompilerError> unexpected() const {
        return std::unexpected<CompilerError>(
            CompilerError{type, line, column, message});
    }

    std::string format() const {
        return std::format("[E{:04d}] {}:{}: {}", error_code(), line, column, message);
    }
};

}