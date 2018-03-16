//
// Created by George on 3/16/18.
//

#ifndef MILES_SSTABLE_H
#define MILES_SSTABLE_H


#include <utility>

#include "storage.h"
#include "key_storage.h"
#include "key_map.h"


class SSTable: public IKeyStorage {

public:

    FetchResult fetch(std::string key) override;

    static std::unique_ptr<SSTable> createFromKeyMap(const KeyMap& km, std::string fileName);


private:

    explicit SSTable(std::string fileName, std::fstream file):
            fileName(std::move(fileName)),
            file(std::move(file)) {;}

    const std::string fileName;

    std::fstream file;

};


/*
class SSTable: public IKeyStorage {

public:

    explicit SSTable(std::string directory): directory(std::move(directory)) {;}

    FetchResult fetch(std::string key) override;

private:

    //StoreResult appendKey(std::string key, std::unique_ptr<json> payload);

    const std::string directory;

    const std::string prefix;

    const std::unique_ptr<std::vector<std::pair<std::string, std::fstream>>> tableFiles = std::make_unique<std::vector<std::pair<std::string, std::fstream>>>(); //   std::unique_ptr<std::unordered_map<std::string, std::fstream>> tableMap = std::make_unique<std::unordered_map<std::string, std::fstream>>();

};
*/




#endif //MILES_SSTABLE_H
