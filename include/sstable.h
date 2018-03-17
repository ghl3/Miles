//
// Created by George on 3/16/18.
//

#ifndef MILES_SSTABLE_H
#define MILES_SSTABLE_H

#include <fstream>
#include <utility>

#include "key_storage.h"
#include "key_map.h"


class SSTable: public IKeyStorage {

public:

    FetchResult fetch(std::string key) override;

    static std::unique_ptr<SSTable> createFromKeyMap(const KeyMap& km, std::string fileName);

    static std::unique_ptr<SSTable> createFromFileName(std::string fileName);


private:

    explicit SSTable(std::string fileName, std::unique_ptr<std::fstream> file):
            fileName(std::move(fileName)),
            file(std::move(file)) {;}

    const std::string fileName;

    std::unique_ptr<std::fstream> file;

};

#endif //MILES_SSTABLE_H
