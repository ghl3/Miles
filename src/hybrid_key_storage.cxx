//
// Created by George on 3/16/18.
//


#include <results.h>
#include <iostream>

#include "hybrid_key_storage.h"


StoreResult HybridKeyStorage::store(std::string key, std::unique_ptr<json> payload) {

    std::lock_guard<std::mutex> guard(this->lock);

    wal.log(key, payload.get());

    // If the key is already in the in-memory storage, we
    //
    if (this->inMemoryStorage->containsKey(key)) {
        return this->inMemoryStorage->store(key, std::move(payload));
    }

    // If the in-memory storage is at the max size,
    // move it to disk and create a fresh in-memory storage
    if (this->inMemoryStorage->size() >= this->maxInMemorySize) {
        std::cout << "Moving in-memory map to new SSTable" << std::endl;
        this->moveInMemoryToDisk();
        this->wal.clear();
        this->inMemoryStorage = std::make_unique<KeyMap>();
    }

    // Save the key to in-memory storage
    return this->inMemoryStorage->store(key, std::move(payload));
}


FetchResult HybridKeyStorage::fetch(std::string key) {

    std::lock_guard<std::mutex> guard(this->lock);

    // First, check the in-memory storage
    auto inMemoryResult = this->inMemoryStorage->fetch(key);
    if (inMemoryResult.success) {
        return inMemoryResult;
    } else {

        // Iterate in reverse order
        for(auto rit = diskStorage.rbegin(); rit != diskStorage.rend(); ++rit) {
            auto sstableResult = (*rit)->fetch(key);
            if (sstableResult.success) {
                return sstableResult;
            }
        }

        return FetchResult(false);
    }
}


bool HybridKeyStorage::moveInMemoryToDisk() {
    std::string fileName = (std::stringstream() << directory << "/" << "table_" << this->diskStorage.size() << ".dat").str();
    auto newSSTable = SSTable::createFromKeyMap(*inMemoryStorage, fileName);
    this->diskStorage.push_back(std::move(newSSTable));
    std::cout << "Created new SSTable " << fileName << std::endl;
    return true;
}
