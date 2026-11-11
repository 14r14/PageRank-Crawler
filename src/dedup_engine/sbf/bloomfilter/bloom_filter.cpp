#include "bloom_filter.h"

size_t BloomFilter::calculateBitArraySize(const size_t elements, const float fp_rate)
{
    /*
    Return the size of bit array(m) to used using
    following formula
    m = -(n * lg(p)) / (lg(2)^2)
    n : int
        number of items expected to be stored in filter
    p : float
        False Positive probability in decimal
    */

    return static_cast<size_t>(std::ceil(-(elements * std::log(fp_rate)) / (std::log(2) * std::log(2))));
}

size_t BloomFilter::calculateHashFunctions(const size_t elements)
{
    /*
    Return the hash function(k) to be used using
    following formula
    k = (m/n) * lg(2)

    m : int
        size of bit array
    n : int
        number of items expected to be stored in filter
    */

    return static_cast<size_t>(std::ceil(((double) this->bit_array_size / elements) * std::log(2)));
}

BloomFilter::BloomFilter(const size_t max_elements, const float fp_rate)
    : max_elements(max_elements),
      fp_rate(fp_rate)
{
    size_t bitArraySize = calculateBitArraySize(max_elements, fp_rate);
    this->bit_array_size = bitArraySize;
    this->hash_count = calculateHashFunctions(max_elements);
    this->bit_array.resize(bitArraySize);
    this->bit_array.reset();
}

void BloomFilter::add(XXH_INLINE_XXH128_hash_t &hashed_url)
{
    XXH64_hash_t h1 = hashed_url.low64;
    XXH64_hash_t h2 = hashed_url.high64;

    for (size_t i = 0; i<this->hash_count; i++) {
        size_t digest = (h1 + i * h2) % this->bit_array_size;
        this->bit_array.set(digest, true);
    }
    this->num_elements++;
}

bool BloomFilter::check(XXH_INLINE_XXH128_hash_t &hashed_url) const {
    XXH64_hash_t h1 = hashed_url.low64;
    XXH64_hash_t h2 = hashed_url.high64;
    for (size_t i = 0; i<this->hash_count; i++) {
        size_t digest = (h1 + i * h2) % this->bit_array_size;
        if (!(this->bit_array.at(digest))) return false;
    }

    return true;
}

bool BloomFilter::is_full() {
    return this->num_elements >= max_elements;
}