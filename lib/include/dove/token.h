#pragma once

#include <cstdint>
#include <string_view>

namespace Dove {

/**
 * TokenType
 */
enum class TokenType : uint8_t {
    ValueIdentifier,
    ValueInteger,
    ValueFloatingPointNumber,
    ValueString,
    ValueCharacter,

    TypeI8,
    TypeI16,
    TypeI32,
    TypeI64,
    TypeI128,
    TypeU8,
    TypeU16,
    TypeU32,
    TypeU64,
    TypeU128,
    TypeF64,
    TypeF128,
    TypeCh,
    TypeBool,

    KeywordLet,
    KeywordObj,
    KeywordConst,
    KeywordFunc,
    KeywordUse,
    KeywordIf,
    KeywordElse,
    KeywordElif,
    KeywordMatch,
    KeywordIs,
    KeywordFallback,
    KeywordLoop,
    KeywordFor,
    KeywordIn,
    KeywordWhile,
    KeywordBrk,
    KeywordRtn,
    KeywordTrue,
    KeywordFalse,

    SymbolAnd,
    SymbolAmpersand,
    SymbolOr,
    SymbolVerticalBar,
    SymbolAssign,
    SymbolEqual,
    SymbolNot,
    SymbolNotEqual,
    SymbolPlus,
    SymbolPlusEqual,
    SymbolMinus,
    SymbolMinusEqual,
    SymbolArrow,
    SymbolAsterisk,
    SymbolAsteriskEqual,
    SymbolSlash,
    SymbolSlashEqual,
    SymbolModulo,
    SymbolModuloEqual,
    SymbolLess,
    SymbolLessEqual,
    SymbolShiftLeft,
    SymbolGreater,
    SymbolGreaterEqual,
    SymbolShiftRight,
    SymbolColon,
    SymbolDoubleColon,
    SymbolSemicolon,
    SymbolTilde,
    SymbolDot,
    SymbolComma,
    SymbolLeftRoundBracket,
    SymbolRightRoundBracket,
    SymbolLeftSquareBracket,
    SymbolRightSquareBracket,
    SymbolLeftCurlyBracket,
    SymbolRightCurlyBracket,

    PrefixBinary,
    PrefixOctal,
    PrefixHexadecimal,
};

struct Token {
    TokenType type;
    std::string_view str;
    uint32_t line;
    uint32_t column;
};

}