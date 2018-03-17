//
// Created by George on 3/16/18.
//


#include <results.h>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/concept_check.hpp>
#include <boost/regex.hpp>


#include "hybrid_key_storage.h"

namespace fs = ::boost::filesystem;


StoreResult HybridKeyStorage::store(std::string key, std::unique_ptr<json> payload) {

    std::lock_guard<std::mutex> guard(this->lock);

    wal->log(key, *payload);

    // If the key is already in the in-memory storage, we
    //
    if (this->inMemoryStorage->containsKey(key)) {
        return this->inMemoryStorage->store(key, std::move(payload));
    }

    // If the in-memory storage is at the max size,
    // move it to disk and create a fresh in-memory storage
    if (this->inMemoryStorage->size() >= this->maxInMemorySize) {
        std::cout << "Moving in-memory map to new SSTable" << std::endl;
        this->moveInMemoryToDisk();
        this->wal->clear();
        this->inMemoryStorage = std::make_unique<KeyMap>();
    }

    // Save the key to in-memory storage
    return this->inMemoryStorage->store(key, std::move(payload));
}


FetchResult HybridKeyStorage::fetch(std::string key) {

    std::lock_guard<std::mutex> guard(this->lock);

    // First, check the in-memory storage
    auto inMemoryResult = this->inMemoryStorage->fetch(key);
    if (inMemoryResult.success) {
        return inMemoryResult;
    } else {

        // Iterate in reverse order
        for(auto rit = diskStorage.rbegin(); rit != diskStorage.rend(); ++rit) {
            auto sstableResult = (*rit)->fetch(key);
            if (sstableResult.success) {
                return sstableResult;
            }
        }

        return FetchResult(false);
    }
}


bool HybridKeyStorage::moveInMemoryToDisk() {
    std::string fileName = (std::stringstream() << directory << "/" << "table_" << this->diskStorage.size() << ".dat").str();
    auto newSSTable = SSTable::createFromKeyMap(*inMemoryStorage, fileName);
    this->diskStorage.push_back(std::move(newSSTable));
    std::cout << "Created new SSTable " << fileName << std::endl;
    return true;
}


std::vector<std::string> HybridKeyStorage::getDataFiles(std::string directory) {

    const boost::regex my_filter( "table.*\\.dat" );

    fs::directory_iterator it(directory); // + "/table_*.dat");
    fs::directory_iterator endit;
    std::vector<std::string> ret;

    while(it != endit) {

        boost::match_results<std::string::const_iterator> what;

        if (boost::regex_match(it->path().filename().string(), what, my_filter)) {
            ret.push_back(it->path().string());
        }
        ++it;
    }

    return ret;
}


std::unique_ptr<HybridKeyStorage> HybridKeyStorage::buildFromDirectory(std::string directory, size_t maxInMemorySize) {

    auto ptr = std::make_unique<HybridKeyStorage>(directory, maxInMemorySize);

    for (const auto &path: HybridKeyStorage::getDataFiles(directory)) {
        ptr->diskStorage.push_back(SSTable::createFromFileName(path));
    }

    auto keyMapAndWal = Wal::buildKeyMapAndWall(directory + "/wal.log");

    ptr->inMemoryStorage = std::move(keyMapAndWal.second);
    ptr->wal = std::move(keyMapAndWal.first);

    return ptr;
}
