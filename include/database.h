//
// Created by George on 3/12/18.
//

#include <fstream>
#include <string>
#include <unordered_map>
#include <utility>

#include "json.h"
#include "results.h"
#include "table.h"

#ifndef MILES_STORAGE_H
#define MILES_STORAGE_H

using json = nlohmann::json;


class Database {

  public:
    explicit Database(std::string directory, size_t maxStorageSize)
        : maxStorageSize(maxStorageSize), directory(std::move(directory)) {
        ;
    }

    StoreResult store(const std::string& table, const std::string& key, std::vector<char>&& payload);

    StoreResult storeJson(const std::string& table, const std::string& key, const json& data) {
        return this->store(table, key, utils::jsonToCharVector(data));
    }

    FetchResult fetch(const std::string& table, const std::string& key);

  private:
    const size_t maxStorageSize;

    const std::string directory;

    std::unordered_map<std::string, std::unique_ptr<Table>> tableMap;
};

#endif // MILES_STORAGE_H