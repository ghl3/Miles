//
// Created by George on 3/12/18.
//

#include <string>
#include <utility>
#include <unordered_map>

#include "json.h"

#ifndef MILES_STORAGE_H
#define MILES_STORAGE_H

#endif //MILES_STORAGE_H

using json = nlohmann::json;

class StoreResult {

public:

    explicit StoreResult(bool r) : result(r) {;}
    const bool result;

};


class FetchResult {

public:

    explicit FetchResult(bool r) : result(r), payload(nullptr) {;}
    explicit FetchResult(bool r, std::shared_ptr<json> j) : result(r), payload(j) {;}

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
    virtual StoreResult store(std::string, std::string, std::unique_ptr<json>)=0;
    virtual FetchResult fetch(std::string, std::string)=0;
};




class MapStorage: public IStorage {

public:

    StoreResult store(std::string table, std::string key, std::unique_ptr<json> payload) override {

        if(data->find(table) == data->end()) {
            data->insert({table, std::unordered_map<std::string, std::shared_ptr<json>>()});
        }

        //std::shared_ptr<json> s_ptr{std::move(payload)};

        // The underlying storage takes ownership of the JSON
        (*data)[table][key] = std::move(payload); //s_ptr;

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

    std::unique_ptr<std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<json>>>> data = std::make_unique<std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<json>>>>();

};