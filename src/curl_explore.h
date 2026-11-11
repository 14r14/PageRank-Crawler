#pragma once
#include <vector>
#include <string>
#include "../utils/curl_utils/curlops.h"


namespace curl_explore
{
    void exploreSeedUrls(const std::vector<std::string> &seedUrls, std::vector<std::string> &htmlBuffers);
}