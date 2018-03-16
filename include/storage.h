//
// Created by George on 3/12/18.
//

#include <string>
#include <utility>
#include <fstream>
#include <unordered_map>

#include "json.h"
#include "results.h"
#include "hybrid_key_storage.h"

#ifndef MILES_STORAGE_H
#define MILES_STORAGE_H

using json = nlohmann::json;



class Storage {


public:

    explicit Storage(std::string directory, size_t maxStorageSize):
            maxStorageSize(maxStorageSize),
            directory(directory)
    {;}

    StoreResult store(std::string table, std::string key,  std::unique_ptr<json> payload);

    FetchResult fetch(std::string table, std::string key);

private:

    const size_t maxStorageSize;

    const std::string directory;

    std::unordered_map<std::string, std::unique_ptr<HybridKeyStorage>> tableMap;

};

#endif //MILES_STORAGE_H