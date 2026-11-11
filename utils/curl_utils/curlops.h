#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <curl/curl.h>
#include <stdexcept>

namespace utils::curl
{
    std::string fetchAndDownloadHtml(const std::string &url);
    CURL *initEasyHandle(const std::string &url, std::string *htmlBuf);

    namespace
    {
        size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
    }
}