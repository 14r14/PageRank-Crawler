#include "dom_utils.h"
#include "../url_utils/url_utils.h"
#include <unordered_set>

lxb_html_document_t *parseHtmlString(std::string &htmlBuf)
{
    lxb_html_document_t *document = lxb_html_document_create();
    if (document == nullptr)
    {
        std::clog << "Failed to create Lexbor document." << std::endl;
        return nullptr;
    }

    lxb_status_t status = lxb_html_document_parse(
        document,
        reinterpret_cast<const lxb_char_t *>(htmlBuf.data()),
        htmlBuf.size());
    if (status != LXB_STATUS_OK)
    {
        std::cerr << "Failed to parse html string." << std::endl;
        lxb_html_document_destroy(document);
        return nullptr;
    }

    return document;
}

namespace utils::dom
{
    void extractAllUrls(const lxb_url_t *baseUrl, std::string &htmlBuf, SharedQueue<std::string> proc_queue)
    {
        size_t duplicates = 0;
        lxb_html_document_t *document = parseHtmlString(htmlBuf);
        if (document == nullptr)
        {
            return;
        }

        lxb_dom_collection_t *collection = lxb_dom_collection_make(&document->dom_document, 16);
        if (collection == NULL)
        {
            std::cerr << "Failed to make DOM collection." << std::endl;
            lxb_html_document_destroy(document);
            return;
        }

        if (document->body == nullptr)
        {
            lxb_dom_collection_destroy(collection, false);
            lxb_html_document_destroy(document);
            return;
        }

        const lxb_char_t *tagName = (const lxb_char_t *)"a";
        size_t tagLength = 1;

        lxb_status_t status = lxb_dom_elements_by_tag_name(
            lxb_dom_interface_element(document->body),
            collection,
            tagName,
            tagLength);
        if (status != LXB_STATUS_OK)
        {
            std::cerr << "Failed to find elements by tag name." << std::endl;
            lxb_dom_collection_destroy(collection, false);
            lxb_html_document_destroy(document);
            return;
        }
        size_t count = lxb_dom_collection_length(collection);
        lxb_url_parser_t *parser = lxb_url_parser_create();
        if (parser == nullptr || lxb_url_parser_init(parser, nullptr) != LXB_STATUS_OK)
        {
            lxb_url_parser_destroy(parser, true);
            lxb_dom_collection_destroy(collection, false);
            lxb_html_document_destroy(document);
            std::cerr << "Failed to initialize Lexbor URL parser." << std::endl;
            return;
        }

        std::vector<std::string> allValidUrls;

        for (size_t i = 0; i < count; i++)
        {
            lxb_dom_element_t *element = lxb_dom_collection_element(collection, i);
            const lxb_char_t *attr_name = (const lxb_char_t *)"href";
            size_t attr_len = 4;

            if (element != nullptr)
            {
                lxb_dom_attr_t *attr = lxb_dom_element_attr_by_name(element, attr_name, attr_len);
                if (attr != nullptr)
                {
                    size_t raw_len = 0;
                    const lxb_char_t *value = lxb_dom_attr_value(attr, &raw_len);

                    if (value != nullptr && raw_len > 0)
                    {
                        std::string_view href(reinterpret_cast<const char *>(value), raw_len);

                        auto [resource_url, fragment] = utils::url::separate_fragment(href);

                        if (resource_url.empty())
                        {
                            continue;
                        }

                        lxb_url_parser_clean(parser);
                        lxb_url_t *url = lxb_url_parse(
                            parser,
                            baseUrl,
                            reinterpret_cast<const lxb_char_t *>(resource_url.data()),
                            resource_url.length());

                        if (url != nullptr)
                        {
                            const lexbor_str_t *scheme = lxb_url_scheme(url);
                            if (scheme == nullptr || scheme->data == nullptr)
                            {
                                continue;
                            }
                            const std::string_view schemeStr(
                                reinterpret_cast<const char *>(scheme->data),
                                scheme->length);
                            if (!(utils::string::equals_ignore_case(schemeStr, "http")) && !(utils::string::equals_ignore_case(schemeStr, "https")))
                            {
                                continue;
                            }

                            std::string canonical_url;

                            lxb_status_t status = lxb_url_serialize(
                                url,
                                [](const lxb_char_t *data, size_t len, void *ctx) -> lxb_status_t
                                {
                                    static_cast<std::string *>(ctx)->append(reinterpret_cast<const char *>(data), len);
                                    return LXB_STATUS_OK;
                                },
                                &canonical_url,
                                true);

                            if (status == LXB_STATUS_OK)
                            {
                                utils::url::normalizeUrl(canonical_url);
                                if (!CrawlerDeduplicationEngine::is_duplicate(canonical_url))
                                {
                                    proc_queue->enqueue(canonical_url);
                                }
                                else
                                {
                                    duplicates++;
                                    continue;
                                };
                            }
                        }
                    }
                }
            }
        }

        std::clog << "# of duplicates: " << duplicates << std::endl;

        lxb_dom_collection_destroy(collection, false);
        lxb_url_parser_memory_destroy(parser);
        lxb_url_parser_destroy(parser, true);
        lxb_html_document_destroy(document);
    }
}