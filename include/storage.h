//
// Created by George on 3/12/18.
//

#include <string>
#include <utility>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include "json.h"

#ifndef MILES_STORAGE_H
#define MILES_STORAGE_H

using json = nlohmann::json;

class StoreResult {

public:

    explicit StoreResult(bool r) : result(r) {;}
    const bool result;

};


class FetchResult {

public:

    explicit FetchResult(bool r) : result(r), payload(nullptr) {;}
    explicit FetchResult(bool r, const std::shared_ptr<json>& j) : result(r), payload(j) {;}

    const bool result;

    const json& getJson() {
        return *payload;
    }

private:

    const std::shared_ptr<const json> payload;

};


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

    StoreResult store(std::string table, std::string key, std::unique_ptr<json> payload) override {
        if (this->tableMap->find(table) == this->tableMap->end()) {
            (*this->tableMap)[table] = std::fstream((std::stringstream() << directory << "/" << table << ".dat").str(),
                                                    std::fstream::in | std::fstream::out | std::fstream::trunc);
        }

        (*this->tableMap)[table] << key << std::endl;
        (*this->tableMap)[table] << payload->dump() << std::endl;

        return StoreResult(true);
    }

    FetchResult fetch(std::string table, std::string key) override {

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


private:

    const std::string directory;

    const std::unique_ptr<std::unordered_map<std::string, std::fstream>> tableMap = std::make_unique<std::unordered_map<std::string, std::fstream>>();

};



class MapStorage: public IStorage {

public:

    StoreResult store(std::string table, std::string key, std::unique_ptr<json> payload) override {

        if(data->find(table) == data->end()) {
            data->insert({table, std::unordered_map<std::string, std::shared_ptr<json>>()});
        }

        // The underlying storage takes ownership of the JSON
        (*data)[table][key] = std::move(payload);

        return StoreResult(true);
    }

    FetchResult fetch(std::string table, std::string key) override {

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


private:

    const std::unique_ptr<std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<json>>>> data = std::make_unique<std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<json>>>>();

};


#endif //MILES_STORAGE_H