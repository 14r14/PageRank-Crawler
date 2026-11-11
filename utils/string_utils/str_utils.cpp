#include "str_utils.h"

bool utils::string::equals_ignore_case(std::string_view lhs, std::string_view rhs) {
    return std::ranges::equal(lhs, rhs, [](unsigned char a, unsigned char b) {
        return std::tolower(a) == std::tolower(b);
    });
}

std::array<char, 32> utils::string::hash_to_string(XXH128_hash_t hash) {
    static constexpr char hex_chars[] = "0123456789abcdef";
    std::array<char, 32> out;

    // Direct nibble extraction guarantees exact 32 characters with leading zeros
    for (int i = 15; i >= 0; --i) {
        out[15 - i] = hex_chars[(hash.high64 >> (i * 4)) & 0x0F];
        out[31 - i] = hex_chars[(hash.low64 >> (i * 4)) & 0x0F];
    }
    return out;
}