//
// Created by George on 3/13/18.
//

#include "database.h"
#include <sstream>

#include "results.h"

StoreResult Database::store(const std::string& table, const std::string& key, std::vector<char>&& payload) {

    if (this->tableMap.find(table) == tableMap.end()) {

        auto tableDirName = (std::stringstream() << this->directory << "/" << table).str();

        (this->tableMap)[table] = std::make_unique<Table>(tableDirName, maxStorageSize);
    }

    return (this->tableMap)[table]->store(key, std::move(payload));
}

FetchResult Database::fetch(const std::string& table, const std::string& key) {
    if (this->tableMap.find(table) == this->tableMap.end()) {
        return FetchResult::error(ResultType::TABLE_NOT_FOUND);
    } else {
        return (this->tableMap)[table]->fetch(key);
    }
}
