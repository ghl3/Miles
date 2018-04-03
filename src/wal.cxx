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

bool Wal::log(const std::string &key, const std::vector<char> &payload) {
    this->file.clear();
    this->file << key << std::endl;
    this->file.write(&(payload[0]), payload.size());
    this->file << std::endl;
    return true;
}

bool Wal::logJson(const std::string &key, const json &payload) {
    return Wal::log(key, utils::stringToCharVector(payload.dump()));
}

bool Wal::clear() {
    auto tmp = std::fstream(path, std::fstream::in | std::fstream::out |
                                      std::fstream::trunc);
    file = std::fstream(path, std::fstream::in | std::fstream::out |
                                  std::fstream::ate);
    return true;
}

std::pair<std::unique_ptr<Wal>, std::unique_ptr<KeyMap>>
Wal::buildKeyMapAndWall(std::string walPath) {

    auto wal = std::make_unique<Wal>(walPath);
    auto keyMap = std::make_unique<KeyMap>();

    for (std::istream_iterator<std::string> it = wal->begin(); it != wal->end();
         ++it) {
        auto key = *it;
        ++it;
        std::string val = *it;
        keyMap->store(key, utils::stringToCharVector(val));
    }

    return std::make_pair(std::move(wal), std::move(keyMap));
}
