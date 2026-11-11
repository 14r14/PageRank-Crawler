#include "CLI11.hpp"
#include "../utils/file_utils/fileio.h"
#include "../utils/dom_utils/dom_utils.h"
#include "curl_explore.h"

void extractSeedUrls(const std::string& fileName, std::vector<std::string>& seedUrls)
{
    auto rawLines = utils::file::readAllLines(fileName);
    seedUrls.reserve(rawLines.size());

    for (const auto& line: rawLines) {
        if (auto pStr = std::get_if<std::string>(&line)) {
            seedUrls.push_back(*pStr);
        }   
    }
}

int main(int argc, char* argv[]) {
    CLI::App app{"PageRank Crawler Engine"};

    std::string filename;

    // Define arguments
    app.add_option("-f,--file", filename, "Path to the seed urls file")->required();

    CLI11_PARSE(app, argc, argv);

    std::clog << "Seed File: " << filename << std::endl;

    std::vector<std::string> seedUrls;

    extractSeedUrls(filename, seedUrls);

    std::vector<std::string> htmlBuffers(seedUrls.size());

    curl_explore::exploreSeedUrls(seedUrls, htmlBuffers);

    if (seedUrls.empty())
    {
        std::cerr << "No seed URLs were provided." << std::endl;
        return 1;
    }

    auto processing_queue = std::make_shared<moodycamel::ConcurrentQueue<std::string>>();

    lxb_url_parser_t *urlParser = lxb_url_parser_create();
    if (urlParser == nullptr || lxb_url_parser_init(urlParser, nullptr) != LXB_STATUS_OK)
    {
        lxb_url_parser_destroy(urlParser, true);
        std::cerr << "Failed to initialize Lexbor URL parser." << std::endl;
        return 1;
    }

    lxb_url_t *baseUrl = lxb_url_parse(
        urlParser,
        nullptr,
        reinterpret_cast<const lxb_char_t *>(seedUrls[0].data()),
        seedUrls[0].size()
    );
    if (baseUrl == nullptr)
    {
        lxb_url_parser_memory_destroy(urlParser);
        lxb_url_parser_destroy(urlParser, true);
        std::cerr << "Failed to parse seed URL." << std::endl;
        return 1;
    }

    utils::dom::extractAllUrls(baseUrl, htmlBuffers[0], processing_queue);

    lxb_url_parser_memory_destroy(urlParser);
    lxb_url_parser_destroy(urlParser, true);

    return 0;
}