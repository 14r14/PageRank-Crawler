#include "url_db.h"
#include <stdexcept>

void UrlDB::create_connection() {
    rocksdb::Options options;
    options.create_if_missing = true;

    rocksdb::Status s = rocksdb::DB::Open(options, STORE_DIR, &this->db);
    if (!s.ok()) {
        throw std::runtime_error("Failed to open RocksDB: " + s.ToString());
    }
}

UrlDB::UrlDB() {
    create_connection();
}

UrlDB::~UrlDB() {
    rocksdb::Status s;
    rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
    delete it; 

    const rocksdb::Snapshot* snap = db->GetSnapshot();
    db->ReleaseSnapshot(snap);

    s = db->Close();
    assert(s.ok());
}

void UrlDB::add(std::string_view hashed_url, std::string_view canonical_url) {
    rocksdb::Status s = this->db->Put(rocksdb::WriteOptions(), hashed_url, canonical_url);
    if (!s.ok()) {
        throw std::runtime_error("Failed to write key to RocksDB: " + s.ToString());
    }
};

bool UrlDB::contains(std::string_view hashed_url, std::string *value) const {
    rocksdb::Status s = this->db->Get(rocksdb::ReadOptions(), hashed_url, value);
    if (s.ok()) {
        return true;
    } else if (s.IsNotFound()) {
        return false;
    } else {
        throw std::runtime_error("Failed to get key from RocksDB: " + s.ToString());
    }
}