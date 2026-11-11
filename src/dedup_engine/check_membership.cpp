#include "check_membership.h"

bool CrawlerDeduplicationEngine::is_duplicate(std::string_view canonicalUrl) {
    static UrlDB url_db;
    XXH128_hash_t hashed_url = XXH3_128bits(canonicalUrl.data(), canonicalUrl.size());
    std::array<char, 32> hash = utils::string::hash_to_string(hashed_url);
    const std::string key = std::string(hash.data(), hash.size());
    if(bloomFilter.check(hashed_url)) {
        std::string value;
        if (url_db.contains(key, &value)) {
            if (!utils::string::equals_ignore_case(canonicalUrl, value)) {
                throw std::runtime_error("Hash collision");
            } else {
                return true;
            }
        } else {
            bloomFilter.add(hashed_url);
            url_db.add(key, canonicalUrl);
            return false;
        }
    } else {
        bloomFilter.add(hashed_url);
        url_db.add(key, canonicalUrl);
        return false;
    }
}