#include "curlops.h"

namespace utils::curl
{
    std::string fetchAndDownloadHtml(const std::string &url)
    {
        CURL *curl;
        CURLcode res;
        std::string htmlBuffer;

        curl = curl_easy_init();
        if (!curl)
        {
            std::clog << "Failed to initialize libcurl." << std::endl;
            throw std::runtime_error("Failed to initialize libcurl.");
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &htmlBuffer);

        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        // Perform the request, res will get the return code
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            // Success! The HTML content is now captured in your string
            std::cout << "Successfully retrieved " << htmlBuffer.size() << " bytes." << std::endl;
            std::cout << "--- HTML Content Begin ---" << std::endl;
            std::cout << htmlBuffer << std::endl;
            std::cout << "--- HTML Content End ---" << std::endl;
        }

        curl_easy_cleanup(curl);

        return htmlBuffer;
    }

    CURL *initEasyHandle(const std::string &url, std::string *htmlBuf)
    {
        CURL *curl;

        // Initialize a libcurl easy session
        curl = curl_easy_init();
        if (!curl)
        {
            std::clog << "Failed to initialize libcurl." << std::endl;
            throw std::runtime_error("Failed to initialize libcurl.");
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, htmlBuf);

        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3000);

        return curl;
    }

    namespace
    {
        size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
        {
            size_t totalSize = size * nmemb;
            std::string *s = static_cast<std::string *>(userp);
            s->append(static_cast<char *>(contents), totalSize);
            return totalSize;
        }
    }
}
