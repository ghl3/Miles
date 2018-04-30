//
// Created by George on 3/13/18.
//

#ifndef MILES_RESULTS_H
#define MILES_RESULTS_H

#include "json.h"
#include "utils.h"

using json = nlohmann::json;

enum class ResultType {
    INVALID_TABLE,
    FOUND_IN_MEMTABLE,
    FOUND_IN_SSTABLE,
    DELETED_IN_MEMTABLE,
    DELETED_IN_SSTABLE,
    NOT_FOUND
};

class StoreResult {

  public:
    explicit StoreResult(bool r) : isSuccess(r) { ; }
    const bool isSuccess;
};

class FetchResult {

  public:
    FetchResult(FetchResult&& that) noexcept
        : isPresent(that.isPresent), payload(std::move(that.payload)), resultType(that.resultType) {
        ;
    }

    const bool isPresent;

    const std::vector<char> getPayload() { return payload; }

    const std::string getAsString() { return utils::charVectorToString(this->payload); }

    const json getAsJson() { return json::parse(getAsString()); }

    static FetchResult present(std::vector<char>&& p, ResultType resultType) {
        return FetchResult(true, std::move(p), resultType);
    }

    static FetchResult absent(ResultType source) { return FetchResult(false, std::vector<char>(), source); }

  private:
    explicit FetchResult(bool s, std::vector<char>&& p, ResultType resultType)
        : isPresent(s), payload(std::move(p)), resultType(resultType) {
        ;
    }

    std::vector<char> payload;

    const ResultType resultType;
};

#endif // MILES_RESULTS_H
