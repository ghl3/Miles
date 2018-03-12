//
// Created by George on 3/12/18.
//

#include <string>

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
    explicit FetchResult(bool r, const json& j) : result(r), payload(j) {;}

    const bool result;

private:

    const std::shared_ptr<const json> payload;

};


class IStorage {

public:

    IStorage() = default;
    virtual ~IStorage() = default;
    virtual StoreResult store(std::string, std::string, std::unique_ptr<const json>)=0;
    virtual FetchResult fetch(std::string, std::string)=0;
};