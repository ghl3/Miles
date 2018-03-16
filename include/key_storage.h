//
// Created by George on 3/16/18.
//

#ifndef MILES_KEY_STORAGE_H
#define MILES_KEY_STORAGE_H


#include "results.h"

class IKeyStorage {

public:

    IKeyStorage() = default;
    virtual ~IKeyStorage() = default;

    IKeyStorage(IKeyStorage const &) = delete;
    void operator=(IKeyStorage const &x) = delete;

    //virtual StoreResult store(std::string, std::unique_ptr<json>)=0;
    virtual FetchResult fetch(std::string)=0;
};


#endif //MILES_KEY_STORAGE_H
