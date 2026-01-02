#pragma once

#include <cstdint>
#include <string_view>

namespace Dove {

// Not used anymore..

class Unicode {

public:
    static uint8_t read_unicode(const std::string_view &str, uint32_t idx, uint32_t *out) {
        if (idx >= str.length()) return 0;

        uint32_t codepoint = 0;
        uint8_t bytes_read = 0;
        uint8_t byte1 = static_cast<uint8_t>(str[idx]);

        // 1-byte (0xxxxxxx)
        if ((byte1 & 0x80) == 0) {
            codepoint = byte1;
            bytes_read = 1;
        }
        // 2-bytes (110xxxxx 10xxxxxx)
        else if ((byte1 & 0xE0) == 0xC0) {
            if (idx + 1 >= str.length()) return 0;
            codepoint = ((byte1 & 0x1F) << 6) | (static_cast<uint8_t>(str[idx + 1]));
            bytes_read = 2;
        }
        // 3-bytes (1110xxxx 10xxxxxx 10xxxxxx)
        else if ((byte1 & 0xF0) == 0xE0) {
            if (idx + 2 >= str.length()) return 0;
            codepoint = ((byte1 & 0x0F) << 12) |
                        ((static_cast<uint8_t>(str[idx + 1]) & 0x3F) << 6) |
                        (static_cast<uint8_t>(str[idx + 2]) & 0x3F);
            bytes_read = 3;
        }
        // 4-bytes (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
        else if ((byte1 & 0xF8) == 0xF0) {
            if (idx + 3 >= str.length()) return 0;
            codepoint = ((byte1 & 0x07) << 18) |
                        ((static_cast<uint8_t>(str[idx + 1]) & 0x3F) << 12) |
                        ((static_cast<uint8_t>(str[idx + 2]) & 0x3F) << 6) |
                        (static_cast<uint8_t>(str[idx + 3]) & 0x3F);
            bytes_read = 4;
        }
        // Invalid
        else
            return 0;

        *out = codepoint;
        return bytes_read;
    }
};

}