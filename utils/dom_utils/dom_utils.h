#pragma once
#include <lexbor/html/parser.h>
#include <lexbor/dom/collection.h>
#include <lexbor/dom/interfaces/element.h>
#include <lexbor/url/url.h>
#include <lexbor/html/html.h>
#include <string>
#include <iostream>
#include <string_view>
#include <utility>
#include <ranges>
#include <map>
#include "../string_utils/str_utils.h"
#include "concurrentqueue.h"
#include "../../src/dedup_engine/check_membership.h"

namespace utils::dom {
    template <typename T>
    using SharedQueue = std::shared_ptr<moodycamel::ConcurrentQueue<T>>;

    void extractAllUrls(const lxb_url_t *baseUrl, std::string& htmlBuf, SharedQueue<std::string> proc_queue);
}