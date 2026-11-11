#pragma once
#include <string>
#include <iostream>
#include <string_view>
#include <utility>
#include <ranges>
#include <map>
#include <unordered_set>

struct UrlParts
{
    std::string_view resource_url;
    std::string_view fragment;
};

static const std::unordered_set<std::string_view> TRACKING_PARAMS = {
    "utm_source", "utm_medium", "utm_campaign", "utm_term", "utm_content",
    "gclid", "fbclid", "ref", "source", "mc_cid", "mc_eid"};

namespace utils::url {
    UrlParts separate_fragment(std::string_view raw_href);
    void normalizeUrl(std::string &url);
}