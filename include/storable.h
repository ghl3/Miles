//
// Created by George on 3/16/18.
//

#ifndef MILES_KEY_STORAGE_H
#define MILES_KEY_STORAGE_H

#include "gsl.h"
#include "results.h"

#include "utils.h"

class IStorable {

  public:
    IStorable() = default;
    virtual ~IStorable() = default;

    IStorable(IStorable const&) = delete;
    void operator=(IStorable const& x) = delete;

    virtual StoreResult store(const std::string& key, std::vector<char>&&) = 0;

    virtual StoreResult storeJson(const std::string& key, const json& payload) final {
        return store(key, utils::jsonToCharVector(payload));
    }

    virtual StoreResult storeString(const std::string& key, const std::string& str) final {
        return store(key, utils::stringToCharVector(str));
    }
};

#endif // MILES_KEY_STORAGE_H
