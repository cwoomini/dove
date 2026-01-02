#include "dove/lexer.h"
#include "dove/error.h"
#include "dove/token.h"

#include <unordered_set>
#include <map>

using namespace Dove;

Lexer::Lexer(std::string_view source) : source(source), cursor(0), line(1), column(1) {
    auto res = start();
    if (!res) {
        error = res.error();
    }
}

std::expected<const std::vector<Token> *, CompilerError> Lexer::get_tokens() {
    if (error) return std::unexpected<CompilerError>(error.value());
    return &tokens;
}

std::expected<void, CompilerError> Lexer::start() {
    while (cursor < source.length()) {
        uint8_t ch = peek();
        if (ch == '\0') break;

        uint8_t prev = peek_prev();
        uint8_t next = peek_next();

        switch (ch) {
            case ' ':
            case '\t':
            case '\r': {
                advance();
                break;
            }
            case '\n': {
                advance();
                new_line();
                break;
            }
            case '&': {
                if (prev == '&') {
                    tokens.push_back(Token{.type = TokenType::SymbolAnd,
                                           .str = source.substr(cursor - 1, 2),
                                           .line = line,
                                           .column = column - 1});
                } else if (next != '&') {
                    tokens.push_back(Token{.type = TokenType::SymbolAmpersand,
                                           .str = source.substr(cursor, 1),
                                           .line = line,
                                           .column = column});
                }
                advance();
                break;
            }
            case '|': {
                if (prev == '|') {
                    tokens.push_back(Token{.type = TokenType::SymbolOr,
                                           .str = source.substr(cursor - 1, 2),
                                           .line = line,
                                           .column = column - 1});
                } else if (next != '|') {
                    tokens.push_back(Token{.type = TokenType::SymbolVerticalBar,
                                           .str = source.substr(cursor, 1),
                                           .line = line,
                                           .column = column});
                }
                advance();
                break;
            }
            case '=': {
                if (prev == '=') {
                    tokens.push_back(Token{.type = TokenType::SymbolEqual,
                                           .str = source.substr(cursor - 1, 2),
                                           .line = line,
                                           .column = column - 1});
                } else if (prev == '!') {
                    tokens.push_back(Token{.type = TokenType::SymbolNotEqual,
                                           .str = source.substr(cursor - 1, 2),
                                           .line = line,
                                           .column = column - 1});
                } else if (prev == '+') {
                    tokens.push_back(Token{.type = TokenType::SymbolPlusEqual,
                                           .str = source.substr(cursor - 1, 2),
                                           .line = line,
                                           .column = column - 1});
                } else if (prev == '-') {
                    tokens.push_back(Token{.type = TokenType::SymbolMinusEqual,
                                           .str = source.substr(cursor - 1, 2),
                                           .line = line,
                                           .column = column - 1});
                } else if (prev == '*') {
                    tokens.push_back(Token{.type = TokenType::SymbolAsteriskEqual,
                                           .str = source.substr(cursor - 1, 2),
                                           .line = line,
                                           .column = column - 1});
                } else if (prev == '/') {
                    tokens.push_back(Token{.type = TokenType::SymbolSlashEqual,
                                           .str = source.substr(cursor - 1, 2),
                                           .line = line,
                                           .column = column - 1});
                } else if (prev == '%') {
                    tokens.push_back(Token{.type = TokenType::SymbolModuloEqual,
                                           .str = source.substr(cursor - 1, 2),
                                           .line = line,
                                           .column = column - 1});
                } else if (prev == '<') {
                    tokens.push_back(Token{.type = TokenType::SymbolLessEqual,
                                           .str = source.substr(cursor - 1, 2),
                                           .line = line,
                                           .column = column - 1});
                } else if (prev == '>') {
                    tokens.push_back(Token{.type = TokenType::SymbolGreaterEqual,
                                           .str = source.substr(cursor - 1, 2),
                                           .line = line,
                                           .column = column - 1});
                } else if (next != '=') {
                    tokens.push_back(Token{.type = TokenType::SymbolAssign,
                                           .str = source.substr(cursor, 1),
                                           .line = line,
                                           .column = column});
                }
                advance();
                break;
            }
            case '!': {
                if (next != '=') {
                    tokens.push_back(Token{.type = TokenType::SymbolNot,
                                           .str = source.substr(cursor, 1),
                                           .line = line,
                                           .column = column});
                }
                advance();
                break;
            }
            case '+': {
                if (next != '=') {
                    tokens.push_back(Token{.type = TokenType::SymbolPlus,
                                           .str = source.substr(cursor, 1),
                                           .line = line,
                                           .column = column});
                }
                advance();
                break;
            }
            case '-': {
                if (next != '=' && next != '>') {
                    tokens.push_back(Token{.type = TokenType::SymbolMinus,
                                           .str = source.substr(cursor, 1),
                                           .line = line,
                                           .column = column});
                }
                advance();
                break;
            }
            case '*': {
                if (next != '=') {
                    tokens.push_back(Token{.type = TokenType::SymbolAsterisk,
                                           .str = source.substr(cursor, 1),
                                           .line = line,
                                           .column = column});
                }
                advance();
                break;
            }
            case '/': {
                if (next == '/' || next == '*') {
                    handle_comment();
                    break;
                } else if (next != '=') {
                    tokens.push_back(Token{.type = TokenType::SymbolSlash,
                                           .str = source.substr(cursor, 1),
                                           .line = line,
                                           .column = column});
                }
                advance();
                break;
            }
            case '%': {
                if (next != '=') {
                    tokens.push_back(Token{.type = TokenType::SymbolModulo,
                                           .str = source.substr(cursor, 1),
                                           .line = line,
                                           .column = column});
                }
                advance();
                break;
            }
            case '<': {
                if (prev == '<') {
                    tokens.push_back(Token{.type = TokenType::SymbolShiftLeft,
                                           .str = source.substr(cursor - 1, 2),
                                           .line = line,
                                           .column = column - 1});
                } else if (next != '=' && next != '<') {
                    tokens.push_back(Token{.type = TokenType::SymbolLess,
                                           .str = source.substr(cursor, 1),
                                           .line = line,
                                           .column = column});
                }
                advance();
                break;
            }
            case '>': {
                if (prev == '>') {
                    tokens.push_back(Token{.type = TokenType::SymbolShiftRight,
                                           .str = source.substr(cursor - 1, 2),
                                           .line = line,
                                           .column = column - 1});
                } else if (prev == '-') {
                    tokens.push_back(Token{.type = TokenType::SymbolArrow,
                                           .str = source.substr(cursor - 1, 2),
                                           .line = line,
                                           .column = column - 1});
                } else if (next != '=' && next != '>') {
                    tokens.push_back(Token{.type = TokenType::SymbolGreater,
                                           .str = source.substr(cursor, 1),
                                           .line = line,
                                           .column = column});
                }
                advance();
                break;
            }
            case ':': {
                if (prev == ':') {
                    tokens.push_back(Token{.type = TokenType::SymbolDoubleColon,
                                           .str = source.substr(cursor - 1, 2),
                                           .line = line,
                                           .column = column - 1});
                } else if (next != ':') {
                    tokens.push_back(Token{.type = TokenType::SymbolColon,
                                           .str = source.substr(cursor, 1),
                                           .line = line,
                                           .column = column});
                }
                advance();
                break;
            }
            case ';': {
                tokens.push_back(Token{.type = TokenType::SymbolSemicolon,
                                       .str = source.substr(cursor, 1),
                                       .line = line,
                                       .column = column});
                advance();
                break;
            }
            case '~': {
                tokens.push_back(Token{.type = TokenType::SymbolTilde,
                                       .str = source.substr(cursor, 1),
                                       .line = line,
                                       .column = column});
                advance();
                break;
            }
            case '.': {
                tokens.push_back(Token{.type = TokenType::SymbolDot,
                                       .str = source.substr(cursor, 1),
                                       .line = line,
                                       .column = column});
                advance();
                break;
            }
            case ',': {
                tokens.push_back(Token{.type = TokenType::SymbolComma,
                                       .str = source.substr(cursor, 1),
                                       .line = line,
                                       .column = column});
                advance();
                break;
            }
            case '(': {
                tokens.push_back(Token{.type = TokenType::SymbolLeftRoundBracket,
                                       .str = source.substr(cursor, 1),
                                       .line = line,
                                       .column = column});
                advance();
                break;
            }
            case ')': {
                tokens.push_back(Token{.type = TokenType::SymbolRightRoundBracket,
                                       .str = source.substr(cursor, 1),
                                       .line = line,
                                       .column = column});
                advance();
                break;
            }
            case '[': {
                tokens.push_back(Token{.type = TokenType::SymbolLeftSquareBracket,
                                       .str = source.substr(cursor, 1),
                                       .line = line,
                                       .column = column});
                advance();
                break;
            }
            case ']': {
                tokens.push_back(Token{.type = TokenType::SymbolRightSquareBracket,
                                       .str = source.substr(cursor, 1),
                                       .line = line,
                                       .column = column});
                advance();
                break;
            }
            case '{': {
                tokens.push_back(Token{.type = TokenType::SymbolLeftCurlyBracket,
                                       .str = source.substr(cursor, 1),
                                       .line = line,
                                       .column = column});
                advance();
                break;
            }
            case '}': {
                tokens.push_back(Token{.type = TokenType::SymbolRightCurlyBracket,
                                       .str = source.substr(cursor, 1),
                                       .line = line,
                                       .column = column});
                advance();
                break;
            }
            case '\'': {
                auto res = handle_character();
                if (!res) return std::unexpected<CompilerError>(res.error());
                advance();
                break;
            }
            case '"': {
                auto res = handle_string();
                if (!res) return std::unexpected<CompilerError>(res.error());
                advance();
                break;
            }
            default: {
                // Numbers (Integer || Floating Point)
                if ((ch - '0') < 10u) {
                    auto res = handle_number();
                    if (!res) return std::unexpected<CompilerError>(res.error());
                }
                // Identifiers && Keywords
                else if ((ch | 0x20) - 'a' < 26u || ch == '_') {
                    auto res = handle_identifier();
                    if (!res) return std::unexpected<CompilerError>(res.error());
                }
            }
        }
    }
    return {};
}

void Lexer::advance() {
    if (cursor < source.length()) {
        cursor++;
        column++;
    }
}

uint8_t Lexer::peek() {
    if (cursor >= source.length()) {
        return 0;
    }
    return static_cast<uint8_t>(source[cursor]);
}

uint8_t Lexer::peek_prev(uint32_t n) {
    if (n > cursor) {
        return 0;
    }
    return static_cast<uint8_t>(source[cursor - n]);
}

uint8_t Lexer::peek_next(uint32_t n) {
    if (cursor + n >= source.length()) {
        return 0;
    }
    return static_cast<uint8_t>(source[cursor + n]);
}

void Lexer::new_line() {
    line++;
    column = 1;
}

std::expected<void, CompilerError> Lexer::handle_identifier() {
    uint32_t start_col = column;
    uint32_t start_idx = cursor;

    while (cursor < source.length()) {
        uint8_t ch = peek();

        bool is_valid = ((ch | 0x20) - 'a') < 26u || // a-z, A-Z
                        (ch - '0') < 10u ||          // 0-9
                        ch == '_';                   // _

        if (is_valid) {
            advance();
        } else {
            break;
        }
    }

    std::string_view value = source.substr(start_idx, cursor - start_idx);
    TokenType type = match_token_type(value);

    tokens.push_back(Token{.type = type, .str = value, .line = line, .column = start_col});
    return {};
}

std::expected<void, CompilerError> Lexer::handle_string() {
    uint32_t start_col = column;
    uint32_t start_idx = cursor;

    advance(); // "
    while (cursor < source.length()) {
        uint8_t ch = peek();
        uint8_t prev = peek_prev();

        if (prev != '\\' && ch == '"') {
            break;
        } else if (ch == '\n' || ch == '\0') {
            return CompilerError(LexerError::StringNotTerminated, line, start_col,
                                 "Strings should end with a double quote. Multi-line strings "
                                 "are not yet supported.")
                .unexpected();
        }
        advance();
    }
    std::string_view value = source.substr(start_idx + 1, cursor - start_idx - 1);
    tokens.push_back(
        Token{.type = TokenType::ValueString, .str = value, .line = line, .column = start_col});
    return {};
}

std::expected<void, CompilerError> Lexer::handle_character() {
    uint32_t start_col = column;
    uint32_t start_idx = cursor;
    uint8_t len = 0;

    advance(); // '
    while (cursor < source.length()) {
        uint8_t ch = peek();
        uint8_t prev = peek_prev();
        uint8_t next = peek_next();

        if (prev != '\\' && ch == '\'') {
            if (len == 0) {
                return CompilerError(LexerError::EmptyCharacterLiteral, line, start_col,
                                     "Character value is empty.")
                    .unexpected();
            } else if (len >= 2) {
                return CompilerError(LexerError::ExpectedCharNotString, line, start_col,
                                     "A single character should be written between single-quotes.")
                    .unexpected();
            }
            break;
        }

        if (ch == '\\') {
            static std::unordered_set<uint8_t> escape_chars = {'a', 'b', 'e',  'f',  'n', 'r',
                                                               't', 'v', '\\', '\'', '"', '?'};
            if (escape_chars.find(next) != escape_chars.end()) {
                advance(); // Skip backslash
            }
        }

        advance();
        len++;
    }

    std::string_view value = source.substr(start_idx + 1, cursor - start_idx - 1);
    tokens.push_back(
        Token{.type = TokenType::ValueCharacter, .str = value, .line = line, .column = start_col});
    return {};
}

std::expected<void, CompilerError> Lexer::handle_number() {
    uint32_t start_col = column;
    uint32_t start_idx = cursor;
    bool is_floating_point = false;

    // there's gotta be a better way...
    while (cursor < source.length()) {
        uint8_t ch = peek();
        if ((ch - '0') < 10u || (ch == '.' && !is_floating_point && (peek_next() - '0') < 10u)) {
            if (ch == '.') {
                is_floating_point = true;
            }
            advance();
        } else {
            break;
        }
    }

    std::string_view value = source.substr(start_idx, cursor - start_idx);
    tokens.push_back(Token{.type = is_floating_point ? TokenType::ValueFloatingPointNumber
                                                     : TokenType::ValueInteger,
                           .str = value,
                           .line = line,
                           .column = start_col});
    return {};
}

void Lexer::handle_comment() {
    if (peek_next() == '/') {
        while (cursor < source.length()) {
            if (peek() == '\n' || peek() == '\0') {
                break;
            }
            advance();
        }
    } else if (peek_next() == '*') {
        advance();
        advance();
        while (cursor < source.length()) {
            if (peek_prev() == '*' && peek() == '/') {
                advance();
                break;
            }
            if (peek() == '\n') {
                new_line();
            }
            advance();
        }
    }
}

TokenType Lexer::match_token_type(std::string_view str) {
    static const std::map<std::string_view, TokenType> keywords = {
        {      "i8",          TokenType::TypeI8},
        {     "i16",         TokenType::TypeI16},
        {     "i32",         TokenType::TypeI32},
        {     "i64",         TokenType::TypeI64},
        {    "i128",        TokenType::TypeI128},
        {      "u8",          TokenType::TypeU8},
        {     "u16",         TokenType::TypeU16},
        {     "u32",         TokenType::TypeU32},
        {     "u64",         TokenType::TypeU64},
        {    "u128",        TokenType::TypeU128},
        {     "f64",         TokenType::TypeF64},
        {    "f128",        TokenType::TypeF128},
        {      "ch",          TokenType::TypeCh},
        {    "bool",        TokenType::TypeBool},
        {    "true",     TokenType::KeywordTrue},
        {   "false",    TokenType::KeywordFalse},
        {     "let",      TokenType::KeywordLet},
        {     "obj",      TokenType::KeywordObj},
        {   "const",    TokenType::KeywordConst},
        {    "func",     TokenType::KeywordFunc},
        {     "use",      TokenType::KeywordUse},
        {      "if",       TokenType::KeywordIf},
        {    "elif",     TokenType::KeywordElif},
        {    "else",     TokenType::KeywordElse},
        {   "match",    TokenType::KeywordMatch},
        {      "is",       TokenType::KeywordIs},
        {"fallback", TokenType::KeywordFallback},
        {    "loop",     TokenType::KeywordLoop},
        {     "for",      TokenType::KeywordFor},
        {      "in",       TokenType::KeywordIn},
        {   "while",    TokenType::KeywordWhile},
        {     "brk",      TokenType::KeywordBrk},
        {     "rtn",      TokenType::KeywordRtn},
    };

    auto res = keywords.find(str);
    if (res != keywords.end()) {
        return res->second;
    }
    return TokenType::ValueIdentifier;
}