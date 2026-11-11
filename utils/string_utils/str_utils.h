#pragma once
#include <string>
#include <string_view>
#include <format>

#define XXH_INLINE_ALL
#include "xxhash.h"

namespace utils::string {
    bool equals_ignore_case(std::string_view lhs, std::string_view rhs);
    std::array<char, 32> hash_to_string(XXH_INLINE_XXH128_hash_t hash);
}
