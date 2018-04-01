//
// Created by George on 4/1/18.
//

#ifndef MILES_FETCHABLE_H
#define MILES_FETCHABLE_H

#include "results.h"

class IFetchable {

public:

    IFetchable() = default;
    virtual ~IFetchable() = default;

    IFetchable(IFetchable const &) = delete;
    void operator=(IFetchable const &x) = delete;

    virtual FetchResult fetch(const std::string&)=0;
};


#endif //MILES_FETCHABLE_H
