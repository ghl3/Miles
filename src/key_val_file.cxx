//
// Created by George on 3/13/18.
//

#include "key_val_file.h"


StoreResult KeyValFile::store(std::string key, std::unique_ptr<json> payload) {

    this->file << key << std::endl;
    this->file << payload->dump() << std::endl;

    return StoreResult(true);}


FetchResult KeyValFile::fetch(std::string key) {

    this->file.seekg(0);
    std::string lineKey;
    std::string linePayload;
    while (std::getline(this->file, lineKey)) {

        std::getline(this->file, linePayload);

        if (lineKey == key) {
            return FetchResult(true, std::make_shared<json>(json::parse(linePayload)));
        }
    }

    return FetchResult(false);}