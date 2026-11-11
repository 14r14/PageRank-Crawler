#pragma once
#include "absl/container/flat_hash_set.h"
#include <rocksdb/db.h>
#include <string_view>
#include "xxhash.h"
#include <memory>

inline const std::string STORE_DIR = "./url_store";

class UrlDB
{
private:
    std::unique_ptr<rocksdb::DB> db;
    void create_connection();

public:
    UrlDB();
    ~UrlDB();
    void add(std::string_view hashed_url, std::string_view canonical_url);
    bool contains(std::string_view hashed_url, std::string *value) const;
};