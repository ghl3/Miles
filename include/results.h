//
// Created by George on 3/13/18.
//

#ifndef MILES_RESULTS_H
#define MILES_RESULTS_H

#include "json.h"

using json = nlohmann::json;


class StoreResult {

public:

    explicit StoreResult(bool r) : isSuccess(r) {;}
    const bool isSuccess;

};


class FetchResult {

public:

    //explicit FetchResult(bool r) : success(r), payload(nullptr) {;}

    FetchResult(const FetchResult& that): isSuccess(that.isSuccess), payload(that.payload) {
      ;
    }

    static FetchResult success(const std::shared_ptr<json>& j) {
        return FetchResult(true, j);
    }

    static FetchResult error() {
        return FetchResult(false, nullptr);
    }

    const bool isSuccess;

    const json& getJson() {
        return *payload;
    }

private:

    explicit FetchResult(bool s, const std::shared_ptr<json>& j) : isSuccess(s), payload(j) {;}

    const std::shared_ptr<const json> payload;

};

#endif //MILES_RESULTS_H
