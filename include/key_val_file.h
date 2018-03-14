//
// Created by George on 3/13/18.
//

#ifndef MILES_KEYVALFILE_H
#define MILES_KEYVALFILE_H

#include <string>
#include <algorithm>
#include <fstream>

#include "json.h"
#include "results.h"

using json = nlohmann::json;


class KeyValFile {

public:

    explicit KeyValFile(std::string fileName):
            fileName(std::move(fileName)),
            file(this->fileName, std::fstream::in | std::fstream::out | std::fstream::trunc) {;}

    StoreResult store(std::string key, std::unique_ptr<json> payload);

    FetchResult fetch(std::string key);

private:
    const std::string fileName;
    std::fstream file;

};

#endif //MILES_KEYVALFILE_H
