//
// Created by George on 3/12/18.
//

#include <string>
#include <utility>

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
    explicit FetchResult(bool r, std::shared_ptr<const json> j) : result(r), payload(std::move(j)) {;}

    const bool result;

private:

    const std::shared_ptr<const json> payload;

};


class IStorage {

public:

    IStorage() = default;
    virtual ~IStorage() = default;
    virtual StoreResult store(std::string, std::string, json)=0;
    virtual FetchResult fetch(std::string, std::string)=0;
};




class MapStorage: public IStorage {

public:

    StoreResult store(std::string, std::string, json) override {
        return StoreResult(true);
    }

    FetchResult fetch(std::string, std::string) override {
        return FetchResult(false);
    }
};