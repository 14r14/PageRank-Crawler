cmake_minimum_required(VERSION 3.20)
project(CrawlerDedupEngine LANGUAGES CXX)

find_package(CURL REQUIRED)
find_package(absl REQUIRED)
find_package(RocksDB CONFIG REQUIRED)

if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew" "/usr/local")
endif()

find_package(lexbor QUIET)

add_executable(crawler)

find_path(LEXBOR_INCLUDE_DIR NAMES lexbor/html/parser.h
    HINTS ${lexbor_INCLUDE_DIRS}
    PATH_SUFFIXES include
)

if(NOT lexbor_FOUND)
    # Manually search for headers and libraries in Homebrew paths
    find_library(LEXBOR_LIBRARY NAMES lexbor)

    if(LEXBOR_INCLUDE_DIR AND LEXBOR_LIBRARY)
        set(lexbor_FOUND TRUE)
        # Create an identical interface target to make your code modern and clean
        add_library(lexbor::lexbor UNKNOWN IMPORTED)
        set_target_properties(lexbor::lexbor PROPERTIES
            IMPORTED_LOCATION "${LEXBOR_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${LEXBOR_INCLUDE_DIR}"
        )
    else()
        message(FATAL_ERROR "Could not find lexbor library or headers via Homebrew paths.")
    endif()
endif()

if(NOT LEXBOR_INCLUDE_DIR)
    message(FATAL_ERROR "Could not find Lexbor headers (lexbor/html/parser.h).")
endif()

target_include_directories(crawler PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${LEXBOR_INCLUDE_DIR}
)
target_compile_features(crawler PRIVATE cxx_std_20)
target_link_libraries(crawler PRIVATE 
    CURL::libcurl
    lexbor::lexbor
    absl::flat_hash_map
    RocksDB::rocksdb
)

target_sources(crawler
    PRIVATE
        src/main.cpp
        utils/file_utils/fileio.cpp
        utils/curl_utils/curlops.cpp
        src/curl_explore.cpp
        utils/dom_utils/dom_utils.cpp
        utils/url_utils/url_utils.cpp
        src/dedup_engine/sbf/bloomfilter/bloom_filter.cpp
        src/dedup_engine/sbf/sbf.cpp
        src/dedup_engine/url_db/url_db.cpp
        src/dedup_engine/check_membership.cpp
        utils/string_utils/str_utils.cpp
)