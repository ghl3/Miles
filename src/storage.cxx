//
// Created by George on 3/13/18.
//

#include <sstream>
#include "storage.h"


StoreResult Storage::store(std::string table, std::string key, std::unique_ptr<json> payload) {

    if (this->tableMap.find(table) == tableMap.end()) {

        auto tableDirName = (std::stringstream() << this->directory << "/" << table).str();

        (this->tableMap)[table] = std::make_unique<HybridKeyStorage>(tableDirName, maxStorageSize);
    }

    return (this->tableMap)[table]->store(key, std::move(payload));
}

FetchResult Storage::fetch(std::string table, std::string key) {
    if(this->tableMap.find(table) == this->tableMap.end()) {
        return FetchResult::error();
    } else {
        return (this->tableMap)[table]->fetch(key);
    }
}
