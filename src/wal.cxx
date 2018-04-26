//
// Created by George on 3/16/18.
//

#include <results.h>
#include <string>
#include <wal.h>

#include <fstream>
#include <iostream>

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <glog/logging.h>


Wal::Wal(const std::string& path) : path(path) {
    // Use a temporary stream to ensure the
    // file exists if it hasn't yet been created
    auto tmpFile = std::ofstream(path, std::ios::app);
    tmpFile.close();
    this->file = std::fstream(path, std::fstream::in | std::fstream::out | std::fstream::ate);
};


bool Wal::log(const std::string& key, const std::vector<char>& payload) {

    // Clear any error flags on the file
    // Note: This does NOT clear the contents of the file!
    this->file.clear();

    this->file << key << std::endl;
    this->file.write(&(payload[0]), payload.size());

    // Writing a 'endl' signals a successful write of a line
    // Any line in the WAL tha doesn't end in an endl is ignored/discarded,
    // meaning the data was not successfully written.
    this->file << std::endl;
    return true;
}

bool Wal::log(const std::string& key, std::string&& payload) {
  return Wal::log(key, utils::stringToCharVector(payload));
}

/**
 * Truncate the WAL and reset it to a clean file
 * @return
 */
bool Wal::clear() {
    auto tmp = std::fstream(path, std::fstream::in | std::fstream::out | std::fstream::trunc);
    file = std::fstream(path, std::fstream::in | std::fstream::out | std::fstream::ate);
    return true;
}

Wal::KeyMapAndWal Wal::buildKeyMapAndWal(std::string walPath) {

    auto wal = std::make_unique<Wal>(walPath);
    auto keyMap = std::make_unique<Memtable>();

    wal->getFile().seekg(0);

    std::string key;
    while (std::getline(wal->getFile(), key)) {

        // If we encounter an EOF while reading a key, we consider the
        // write to be currupt and we discard it
        if (wal->getFile().eof()) {
            LOG(INFO) << "Encountered EOF when reading KEY from WAL.  Discarding" << std::endl;
            break;
        }

        // Read the  Value
        std::string value;
        //auto hasValue = std::getline(wal->getFile(), value);
        std::getline(wal->getFile(), value);

        // If we encounter an EOF when reading the value,
        // we consider the value currupt and we discard both the
        // key and the value (we should encounter a newline before EOF)
        if (wal->getFile().eof()) {
            LOG(INFO) << "Encountered EOF when reading VAL from WAL.  Discarding" << std::endl;
            break;
        }

        keyMap->store(key, utils::stringToCharVector(value));

    }

    return std::make_pair(std::move(wal), std::move(keyMap));
}
