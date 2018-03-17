//
// Created by George on 3/16/18.
//

#include <fstream>

#include <results.h>
#include <sstable.h>


FetchResult SSTable::fetch(std::string key) {

    this->file->seekg(0);
    std::string lineKey;
    std::string linePayload;
    while (std::getline(*file, lineKey)) {

        std::getline(*file, linePayload);

        if (lineKey == key) {
            return FetchResult(true, std::make_shared<json>(json::parse(linePayload)));
        }
    }

    return FetchResult(false);
}


std::unique_ptr<SSTable> SSTable::createFromKeyMap(const KeyMap& km, std::string fileName) {

    // Create a new file
    auto file = std::make_unique<std::fstream>(fileName, std::fstream::in | std::fstream::out | std::fstream::trunc);

    for (auto keyValPair: km) {
        *file << keyValPair.first << std::endl;
        *file << keyValPair.second->dump() << std::endl;
    }

    return std::unique_ptr<SSTable>{new SSTable(fileName, std::move(file))};

}



std::unique_ptr<SSTable> SSTable::createFromFileName(std::string fileName) {

    // Create a new file
    auto file = std::make_unique<std::fstream>(fileName, std::fstream::in | std::fstream::out | std::fstream::trunc);

    return std::unique_ptr<SSTable>{new SSTable(fileName, std::move(file))};
}
