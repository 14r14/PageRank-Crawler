#include "url_utils.h"

namespace utils::url
{
    UrlParts separate_fragment(std::string_view raw_href)
    {
        size_t hash_pos = raw_href.find('#');
        if (hash_pos == std::string_view::npos)
        {
            return {raw_href, {}};
        }

        return {
            raw_href.substr(0, hash_pos),
            raw_href.substr(hash_pos + 1)};
    }

    void collapsePathSlashes(std::string &basePart)
    {
        size_t scheme_end = basePart.find("://");
        size_t path_start = (scheme_end == std::string::npos) ? 0 : scheme_end + 3;

        // Find the first single slash indicating start of path
        size_t first_slash = basePart.find('/', path_start);
        if (first_slash == std::string::npos)
        {
            basePart.push_back('/');
            return;
        }

        // Collapse adjacent slashes inside the path section only
        std::string collapsed;
        collapsed.reserve(basePart.size());
        collapsed.append(basePart, 0, first_slash);

        bool prev_was_slash = false;
        for (size_t i = first_slash; i < basePart.size(); ++i)
        {
            char ch = basePart[i];
            if (ch == '/')
            {
                if (!prev_was_slash)
                {
                    collapsed.push_back(ch);
                    prev_was_slash = true;
                }
            }
            else
            {
                collapsed.push_back(ch);
                prev_was_slash = false;
            }
        }

        basePart = std::move(collapsed);
    }

    std::string normalizeQueryParams(std::string_view queryPart)
    {
        if (queryPart.empty())
            return "";
        if (queryPart.front() == '?')
            queryPart.remove_prefix(1);
        if (queryPart.empty())
            return "";

        std::string_view delim = "&";
        std::map<std::string_view, std::string_view> kvPairs;

        std::string normalizedQueryStr;

        size_t pairStart = 0;
        while (pairStart < queryPart.size())
        {
            size_t pairEnd = queryPart.find(delim, pairStart);
            if (pairEnd == std::string_view::npos)
                pairEnd = queryPart.size();

            if (pairStart == pairEnd)
            {
                pairStart = pairEnd + 1;
                continue;
            }

            std::string_view queryPair = queryPart.substr(pairStart, pairEnd - pairStart);
            size_t splitPos = queryPair.find("=");
            if (splitPos != std::string::npos)
            {
                std::string_view key = queryPair.substr(0, splitPos);
                if (TRACKING_PARAMS.contains(key))
                {
                    pairStart = pairEnd + 1;
                    continue;
                }
                kvPairs[key] = queryPair.substr(splitPos + 1);
            }
            else
            {
                if (TRACKING_PARAMS.contains(queryPair))
                {
                    pairStart = pairEnd + 1;
                    continue;
                }
                kvPairs[queryPair] = {};
            }

            pairStart = pairEnd + 1;
        }
        for (const auto &[key, val] : kvPairs)
        {
            std::string result;
            if (normalizedQueryStr.empty())
            {
                if (val.empty())
                    result = std::format("?{}", key);
                else
                    result = std::format("?{}={}", key, val);
            }
            else
            {
                if (val.empty())
                    result = std::format("&{}", key);
                else
                    result = std::format("&{}={}", key, val);
            }
            normalizedQueryStr.append(result);
        }

        return normalizedQueryStr;
    }

    void normalizeUrl(std::string &url)
    {
        size_t queryStartIdx = url.find('?');
        std::string basePart = (queryStartIdx == std::string::npos) ? url : url.substr(0, queryStartIdx);
        std::string_view queryPart = (queryStartIdx == std::string::npos) ? "" : std::string_view(url).substr(queryStartIdx);

        std::string normalizedQueryStr = normalizeQueryParams(queryPart);
        collapsePathSlashes(basePart);

        url = basePart + normalizedQueryStr;
    }
}