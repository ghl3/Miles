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

    for (std::istream_iterator<std::string> it = wal->begin(); it != wal->end(); ++it) {
        auto key = *it;
        ++it;
        std::string val = *it;
        keyMap->store(key, utils::stringToCharVector(val));
    }

    return std::make_pair(std::move(wal), std::move(keyMap));
}
