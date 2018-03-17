//
// Created by George on 3/16/18.
//


#include <string>
#include <results.h>
#include <wal.h>


#include <iostream>
#include <fstream>

#include <iostream>
#include <iterator>
#include <fstream>
#include <string>


bool Wal::log(std::string key, const json& payload) {
    this->file.clear();
    this->file << key << std::endl;
    this->file << payload.dump() << std::endl;
    return true;
}


bool Wal::clear() {
    return true;
}


std::pair<std::unique_ptr<Wal>, std::unique_ptr<KeyMap>> Wal::buildKeyMapAndWall(std::string walPath) {

    auto wal = std::make_unique<Wal>(walPath);
    auto keyMap = std::make_unique<KeyMap>();

    for(std::istream_iterator<std::string> it = wal->begin(); it != wal->end(); ++it) {
        auto key = *it;
        ++it;
        //auto payload =
        keyMap->store(key, std::make_unique<json>(json::parse(*it)));
    }

   return std::make_pair(std::move(wal), std::move(keyMap));
}
