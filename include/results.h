//
// Created by George on 3/13/18.
//

#ifndef MILES_RESULTS_H
#define MILES_RESULTS_H

#include "json.h"
#include "utils.h"

using json = nlohmann::json;


class StoreResult {

public:

    explicit StoreResult(bool r) : isSuccess(r) {;}
    const bool isSuccess;

};

class FetchResult {

public:

    FetchResult(FetchResult&& that) noexcept: isSuccess(that.isSuccess), payload(std::move(that.payload)) {
      ;
    }

    static FetchResult success(std::vector<char>&& p) {
        return FetchResult(true, std::move(p));
    }

    static FetchResult error() {
        return FetchResult(false, std::vector<char>());
    }

    const bool isSuccess;

    const std::vector<char> getPayload() {
        return payload;
    }

    const std::string getAsString() {
        return utils::charVectorToString(this->payload);
        std::string result(this->payload.begin(), this->payload.end());
        return result;
    }

    const std::string getAsJson() {
        return json::parse(getAsString());
    }

private:

    explicit FetchResult(bool s, std::vector<char>&& p) : isSuccess(s), payload(std::move(p)) {;}

    std::vector<char> payload;

};

#endif //MILES_RESULTS_H
