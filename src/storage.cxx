//
// Created by George on 3/13/18.
//

#include "storage.h"


StoreResult DiskStorage::store(std::string table, std::string key, std::unique_ptr<json> payload) {

    //std::lock_guard<std::mutex> guard(this->diskMutex);

    if (this->tableMap->find(table) == this->tableMap->end()) {
        (*this->tableMap)[table] = std::fstream((std::stringstream() << directory << "/" << table << ".dat").str(),
                                                std::fstream::in | std::fstream::out | std::fstream::trunc);
    }

    (*this->tableMap)[table] << key << std::endl;
    (*this->tableMap)[table] << payload->dump() << std::endl;

    return StoreResult(true);
}

FetchResult DiskStorage::fetch(std::string table, std::string key) {

    if(this->tableMap->find(table) == this->tableMap->end()) {
        return FetchResult(false);
    }

    std::fstream& f = (*this->tableMap)[table];

    f.seekg(0);
    std::string lineKey;
    std::string linePayload;
    while (std::getline(f, lineKey)) {

        std::getline(f, linePayload);

        if (lineKey == key) {
            return FetchResult(true, std::make_shared<json>(json::parse(linePayload))); // keyMap[key]);
        }
    }

    return FetchResult(false);
}


StoreResult MapStorage::store(std::string table, std::string key, std::unique_ptr<json> payload) {

    if(data->find(table) == data->end()) {
        data->insert({table, std::unordered_map<std::string, std::shared_ptr<json>>()});
    }

    // The underlying storage takes ownership of the JSON
    (*data)[table][key] = std::move(payload);

    return StoreResult(true);
}

FetchResult MapStorage::fetch(std::string table, std::string key) {

    if(data->find(table) == data->end()) {
        return FetchResult(false);
    } else {

        auto keyMap = (*data)[table];

        if(keyMap.find(key) == keyMap.end()) {
            return FetchResult(false);
        } else {
            return FetchResult(true, keyMap[key]);
        }
    }
}


bool MapStorage::hasKeyInTable(const std::string& table, const std::string& key) {
    if(data->find(table) == data->end()) {
        return false;
    } else {
        auto keyMap = (*data)[table];
        return !(keyMap.find(key) == keyMap.end());
    }
}


StoreResult HybridStorage::store(std::string table, std::string key, std::unique_ptr<json> payload) {

    std::lock_guard<std::mutex> guard(this->lock);

    // If the key is already in the in-memory storage, we
    //
    if (this->inMemoryStorage->hasKeyInTable(table, key)) {
        return this->inMemoryStorage->store(table, key, std::move(payload));
    }

    // If the in-memory storage is at the max size,
    // move it to disk and create a fresh in-memory storage
    if (this->inMemoryStorage->size() >= this->maxInMemorySize) {
        std::string fileName = (std::stringstream() << directory << "/" << table << ".dat").str(),
        auto newSSTable = SSTable::createFromKeyMap(this->inMemoryStorage) = moveToDisk(this->inMemoryStorage);
        this->diskStorage->add(newDiskStorage);
        this->inMemoryStorage = InMemoryStorage::create();
    }

    // Save the key to in-memory storage
    return this->inMemoryStorage->store(table, key, std::move(payload));
}


FetchResult HybridStorage::fetch(std::string table, std::string key) {

    std::lock_guard<std::mutex> guard(this->lock);

    return this->inMemoryStorage->fetch(table, key);
}

