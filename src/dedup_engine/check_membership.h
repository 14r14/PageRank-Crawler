#pragma once
#include <string_view>
#include "sbf/sbf.h"
#include "url_db/url_db.h"
#include "../../utils/string_utils/str_utils.h"

#define XXH_INLINE_ALL
#include "xxhash.h"

inline SBF bloomFilter = SBF();

namespace CrawlerDeduplicationEngine {
    bool is_duplicate(std::string_view canonical_url);
}