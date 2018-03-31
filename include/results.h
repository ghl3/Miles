//
// Created by George on 3/13/18.
//

#ifndef MILES_RESULTS_H
#define MILES_RESULTS_H

#include "json.h"

//using json = nlohmann::json;


class StoreResult {

public:

    explicit StoreResult(bool r) : isSuccess(r) {;}
    const bool isSuccess;

};

template <class T>
class FetchResult {

public:

    //explicit FetchResult(bool r) : success(r), payload(nullptr) {;}

    FetchResult(FetchResult<T>&& that) noexcept: isSuccess(that.isSuccess), payload(that.payload.release()) {
      ;
    }

    static FetchResult<T> success(std::unique_ptr<T> p) {
        return FetchResult(true, std::move(p));
    }

    static FetchResult<T> error() {
        return FetchResult(false, nullptr);
    }

    const bool isSuccess;

    const T& getPayload() {
        return *payload;
    }

private:

    explicit FetchResult(bool s, std::unique_ptr<T> p) : isSuccess(s), payload(p.release()) {;}

    std::unique_ptr<const T> payload;

};

#endif //MILES_RESULTS_H
