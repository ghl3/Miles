//
// Created by George on 3/16/18.
//

#include <memory>
#include <memtable.h>
#include <results.h>

FetchResult Memtable::fetch(const std::string& key) {

    if (this->deletedKeys.find(key) != this->deletedKeys.end()) {
        return FetchResult::absent(ResultType::DELETED_IN_MEMTABLE);
    } else if (data.find(key) == data.end()) {
        return FetchResult::absent(ResultType::NOT_FOUND);
    } else {
        // This copies the data and returns it to the user
        // This is the semantics we want (it allows the fetcher
        // to own the data)
        return FetchResult::present(std::vector<char>(data[key]), ResultType::FOUND_IN_MEMTABLE);
    }
}

StoreResult Memtable::store(const std::string& key, std::vector<char>&& payload) {
    // The data member takes ownership of the underlying payload data
    this->data.emplace(key, std::move(payload));
    this->deletedKeys.erase(key);
    return StoreResult(true);
}

void Memtable::del(const std::string& key) {
    // If the key is not present, erasing is a no-op
    // However, we still need to store the deletion in the
    // deletedKeys set to avoid a fallback search for
    // they key in the SSTable
    this->data.erase(key);
    this->deletedKeys.insert(key);
}

bool Memtable::containsKey(const std::string& key) const { return !(data.find(key) == data.end()); }
