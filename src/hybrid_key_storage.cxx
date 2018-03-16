//
// Created by George on 3/16/18.
//


#include <results.h>

#include "hybrid_key_storage.h"


StoreResult HybridKeyStorage::store(std::string key, std::unique_ptr<json> payload) {

    std::lock_guard<std::mutex> guard(this->lock);

    // If the key is already in the in-memory storage, we
    //
    if (this->inMemoryStorage->containsKey(key)) {
        return this->inMemoryStorage->store(key, std::move(payload));
    }

    // If the in-memory storage is at the max size,
    // move it to disk and create a fresh in-memory storage
    if (this->inMemoryStorage->size() >= this->maxInMemorySize) {
        std::string fileName = (std::stringstream() << directory << "/" << "foobar" << ".dat").str();
        auto newSSTable = SSTable::createFromKeyMap(*inMemoryStorage, fileName);
        this->diskStorage.push_back(std::move(newSSTable));
        this->inMemoryStorage = std::make_unique<KeyMap>();
    }

    // Save the key to in-memory storage
    return this->inMemoryStorage->store(key, std::move(payload));
}


FetchResult HybridKeyStorage::fetch(std::string key) {
    std::lock_guard<std::mutex> guard(this->lock);
    return this->inMemoryStorage->fetch(key);
}

