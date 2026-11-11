#pragma once
#include <cmath>
#include <string>
#include <boost/dynamic_bitset.hpp>

#define XXH_INLINE_ALL
#include "xxhash.h"

class BloomFilter {
    private:
        size_t max_elements;
        float fp_rate;
        size_t bit_array_size;
        size_t hash_count;
        boost::dynamic_bitset<> bit_array;
        size_t num_elements = 0;

        size_t calculateBitArraySize(const size_t elements, const float fp_rate);
        size_t calculateHashFunctions(const size_t elements);

    public:
        BloomFilter(const size_t elements, const float fp_rate);

        void add(XXH_INLINE_XXH128_hash_t &hashed_url);
        bool check(XXH_INLINE_XXH128_hash_t &hashed_url) const;
        bool is_full();
};