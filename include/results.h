//
// Created by George on 3/13/18.
//

#ifndef MILES_RESULTS_H
#define MILES_RESULTS_H

#include "json.h"

using json = nlohmann::json;


class StoreResult {

public:

    explicit StoreResult(bool r) : success(r) {;}
    const bool success;

};


class FetchResult {

public:

    explicit FetchResult(bool r) : success(r), payload(nullptr) {;}
    explicit FetchResult(bool r, const std::shared_ptr<json>& j) : success(r), payload(j) {;}

    const bool success;

    const json& getJson() {
        return *payload;
    }

private:

    const std::shared_ptr<const json> payload;

};

#endif //MILES_RESULTS_H
