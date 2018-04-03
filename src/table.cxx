//
// Created by George on 3/16/18.
//

#include <boost/concept_check.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <iostream>
#include <results.h>

#include <glog/logging.h>

#include "table.h"

namespace fs = ::boost::filesystem;

Table::~Table() {
    this->saveInMemoryToDisk();
    this->wal->clear();
}

StoreResult Table::store(const std::string &key, std::vector<char> &&payload) {

    std::lock_guard<std::mutex> guard(this->lock);

    // If the key is already in the in-memory storage, we
    // store it at the current location
    if (this->inMemoryStorage->containsKey(key)) {
        wal->log(key, payload);
        return this->inMemoryStorage->store(key, std::move(payload));
    }

    // If the in-memory storage is at the max size,
    // move it to disk and create a fresh in-memory storage
    if (this->inMemoryStorage->size() >= this->maxInMemorySize) {
        LOG(INFO) << "Moving in-memory map to new SSTable" << std::endl;
        this->saveInMemoryToDisk();
        this->wal->clear();
        this->inMemoryStorage = std::make_unique<KeyMap>();
    }

    // Save the key to in-memory storage
    wal->log(key, payload);
    return this->inMemoryStorage->store(key, std::move(payload));
}

FetchResult Table::fetch(const std::string &key) {

    std::lock_guard<std::mutex> guard(this->lock);

    // First, check the in-memory storage
    auto inMemoryResult = this->inMemoryStorage->fetch(key);
    if (inMemoryResult.isSuccess) {
        return inMemoryResult;
    } else {

        // Iterate in reverse order
        for (auto rit = diskStorage.rbegin(); rit != diskStorage.rend();
             ++rit) {
            auto sstableResult = (*rit)->fetch(key);
            if (sstableResult.isSuccess) {
                return sstableResult;
            }
        }

        return FetchResult::error();
    }
}

bool Table::saveInMemoryToDisk() {
    std::string fileName =
        (std::stringstream() << directory << "/"
                             << "table_" << this->diskStorage.size() << ".dat")
            .str();
    auto newSSTable = SSTable::createFromKeyMap(*inMemoryStorage, fileName);
    this->diskStorage.push_back(std::move(newSSTable));
    return true;
}

std::vector<std::string> Table::getDataFiles(std::string directory) {

    const boost::regex my_filter("table.*\\.dat");

    fs::directory_iterator it(directory);
    fs::directory_iterator endit;
    std::vector<std::string> ret;

    while (it != endit) {

        boost::match_results<std::string::const_iterator> what;

        if (boost::regex_match(it->path().filename().string(), what,
                               my_filter)) {
            ret.push_back(it->path().string());
        }
        ++it;
    }

    return ret;
}

std::unique_ptr<Table> Table::buildFromDirectory(std::string directory,
                                                 size_t maxInMemorySize) {

    auto ptr = std::make_unique<Table>(directory, maxInMemorySize);

    // TODO: Ensure data files are returned in the right order (they appear to
    // not be)
    for (const auto &path : Table::getDataFiles(directory)) {
        ptr->diskStorage.push_back(SSTable::createFromFileName(path));
    }

    auto keyMapAndWal = Wal::buildKeyMapAndWall(directory + "/wal.log");

    ptr->inMemoryStorage = std::move(keyMapAndWal.second);
    ptr->wal = std::move(keyMapAndWal.first);

    return ptr;
}
