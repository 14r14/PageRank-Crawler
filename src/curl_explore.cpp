#include "curl_explore.h"

void initalizeHandlesForSeedUrls(const std::vector<std::string> &urls, CURLM *multiHandle, std::vector<std::string> &htmlBuffers, std::vector<CURL *> &urlEasyHandles)
{
    for (int i = 0; i < urls.size(); i++)
    {
        CURL *easyHandle = utils::curl::initEasyHandle(urls[i], &htmlBuffers[i]);
        urlEasyHandles.push_back(easyHandle);
        curl_multi_add_handle(multiHandle, easyHandle);
    }
}

namespace curl_explore
{
    void exploreSeedUrls(const std::vector<std::string> &seedUrls, std::vector<std::string> &htmlBuffers)
    {
        std::vector<CURL *> urlEasyHandles;
        CURLM *curlMulti = curl_multi_init();

        initalizeHandlesForSeedUrls(seedUrls, curlMulti, htmlBuffers, urlEasyHandles);

        int still_running = 1;
        while (still_running)
        {
            try
            {
                CURLMcode mc = curl_multi_perform(curlMulti, &still_running);
                if (mc == CURLM_OK && still_running)
                {
                    curl_multi_poll(curlMulti, nullptr, 0, 1000, nullptr);
                }
            }
            catch (std::exception err)
            {
                std::clog << err.what() << std::endl;
                break;
            }
        }

        for (CURL *handle : urlEasyHandles)
        {
            curl_multi_remove_handle(curlMulti, handle);
            curl_easy_cleanup(handle);
        }

        curl_multi_cleanup(curlMulti);
        curl_global_cleanup();
    }
}