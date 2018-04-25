//
// Created by George on 3/16/18.
//

#include <memory>
#include <memtable.h>
#include <results.h>

FetchResult Memtable::fetch(const std::string& key) {
    if (data.find(key) == data.end()) {
        return FetchResult::error(ResultType::KEY_NOT_FOUND);
    } else {
        // This copies the data and returns it to the user
        // This is the semantics we want (it allows the fetcher
        // to own the data)
        return FetchResult::success(std::vector<char>(data[key]), ResultType::FOUND_IN_MEMTABLE);
    }
}

StoreResult Memtable::store(const std::string& key, std::vector<char>&& payload) {
    // The data member takes ownership of the underlying payload data
    data.emplace(key, std::move(payload));
    return StoreResult(true);
}

bool Memtable::containsKey(const std::string& key) const { return !(data.find(key) == data.end()); }
