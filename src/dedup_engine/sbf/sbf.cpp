#include "sbf.h"

void SBF::check_size_or_extend() {
    if (this->element_counts[filter_idx] >= this->current_max_elements)
    {
        size_t new_max_elements = this->current_max_elements*SCALING_FACTOR;
        this->bloomFilters.push_back(BloomFilter(new_max_elements, P0*std::pow(R, filter_idx+1)));
        this->element_counts.push_back(0);
        this->current_max_elements = new_max_elements;
        this->filter_idx++;
    }
}

void SBF::add(XXH_INLINE_XXH128_hash_t &hashed_url) {
    this->check_size_or_extend();
    this->bloomFilters[filter_idx].add(hashed_url);
    this->element_counts[filter_idx]++;
}

bool SBF::check(XXH_INLINE_XXH128_hash_t &hashed_url) const {
    for (const BloomFilter& bf : this->bloomFilters) {
        if (bf.check(hashed_url)) return true;
    }

    return false;
}