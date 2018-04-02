//
// Created by George on 3/16/18.
//

#ifndef MILES_KEY_STORAGE_H
#define MILES_KEY_STORAGE_H

#include "results.h"
#include "gsl.h"

#include "utils.h"

class IStorable {

public:

    IStorable() = default;
    virtual ~IStorable() = default;

    IStorable(IStorable const &) = delete;
    void operator=(IStorable const &x) = delete;

    virtual StoreResult store(const std::string& key, std::vector<char>&&)=0;

    StoreResult storeJson(const std::string& key, const json& payload) {
        return store(key, utils::jsonToCharVector(payload));
    }

};


#endif //MILES_KEY_STORAGE_H
