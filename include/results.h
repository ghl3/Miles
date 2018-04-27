//
// Created by George on 3/13/18.
//

#ifndef MILES_RESULTS_H
#define MILES_RESULTS_H

#include "json.h"
#include "utils.h"

using json = nlohmann::json;

enum class ResultType {
    FOUND_IN_MEMTABLE,
    // NOT_IN_BLOOMFILTER,
    FOUND_IN_SSTABLE,
    // NOT_IN_SSTABLE,
    TABLE_NOT_FOUND,
    KEY_NOT_FOUND,
};

class StoreResult {

  public:
    explicit StoreResult(bool r) : isSuccess(r) { ; }
    const bool isSuccess;
};

class FetchResult {

  public:
    FetchResult(FetchResult&& that) noexcept
        : isSuccess(that.isSuccess), payload(std::move(that.payload)), resultType(that.resultType) {
        ;
    }

    const bool isSuccess;

    const std::vector<char> getPayload() { return payload; }

    const std::string getAsString() {
        return utils::charVectorToString(this->payload);
        std::string result(this->payload.begin(), this->payload.end());
        return result;
    }

    const json getAsJson() { return json::parse(getAsString()); }

    static FetchResult success(std::vector<char>&& p, ResultType resultType) {
        return FetchResult(true, std::move(p), resultType);
    }

    static FetchResult error(ResultType source) { return FetchResult(false, std::vector<char>(), source); }

  private:
    explicit FetchResult(bool s, std::vector<char>&& p, ResultType resultType)
        : isSuccess(s), payload(std::move(p)), resultType(resultType) {
        ;
    }

    std::vector<char> payload;

    const ResultType resultType;
};

#endif // MILES_RESULTS_H
