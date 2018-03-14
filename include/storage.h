//
// Created by George on 3/12/18.
//

#include <string>
#include <utility>
#include <fstream>
#include <sstream>
#include <unordered_map>


#include "results.h"
#include "json.h"

#ifndef MILES_STORAGE_H
#define MILES_STORAGE_H

using json = nlohmann::json;


class IStorage {

public:

    IStorage() = default;
    virtual ~IStorage() = default;

    IStorage(IStorage const &) = delete;
    void operator=(IStorage const &x) = delete;

    virtual StoreResult store(std::string, std::string, std::unique_ptr<json>)=0;
    virtual FetchResult fetch(std::string, std::string)=0;
};


class DiskStorage: public IStorage {

public:

    explicit DiskStorage(std::string directory): directory(std::move(directory)) {;}

    StoreResult store(std::string table, std::string key, std::unique_ptr<json> payload) override;

    FetchResult fetch(std::string table, std::string key) override;

private:

    const std::string directory;

    const std::unique_ptr<std::unordered_map<std::string, std::fstream>> tableMap = std::make_unique<std::unordered_map<std::string, std::fstream>>();

};


class MapStorage: public IStorage {

public:

    StoreResult store(std::string table, std::string key, std::unique_ptr<json> payload) override;

    FetchResult fetch(std::string table, std::string key) override;

private:

    const std::unique_ptr<std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<json>>>> data = std::make_unique<std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<json>>>>();

};




class HybridStorage: public IStorage {

public:

    explicit HybridStorage(std::string directory): directory(std::move(directory)) {;}

    StoreResult store(std::string table, std::string key, std::unique_ptr<json> payload) override;

    FetchResult fetch(std::string table, std::string key) override;

private:

    std::unique_ptr<MapStorage> inMemoryStorage;

    std::vector<std::unique_ptr<DiskStorage>> diskStorage;

    const std::string directory;

    std::mutex lock;

};


#endif //MILES_STORAGE_H