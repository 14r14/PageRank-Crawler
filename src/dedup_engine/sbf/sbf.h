#pragma once
#include <vector>
#include <cmath>
#include "bloomfilter/bloom_filter.h"

inline float TARGET_FALSE_POSITIVE_RATE = 0.01;
inline size_t INITIAL_BLOOM_FILTER_SIZE = 100000;
inline float R = 0.85;
inline float P0 = TARGET_FALSE_POSITIVE_RATE * (1 - R);
inline size_t SCALING_FACTOR = 2;

class SBF
{
private:
    std::vector<BloomFilter> bloomFilters = {BloomFilter(INITIAL_BLOOM_FILTER_SIZE, P0)};
    std::vector<size_t> element_counts = {0};
    size_t num_elements = 0;
    size_t current_max_elements = 100000;
    size_t filter_idx = 0;

    void check_size_or_extend();

public:
    void add(XXH_INLINE_XXH128_hash_t &hashed_url);
    bool check(XXH_INLINE_XXH128_hash_t &hashed_url) const;
};