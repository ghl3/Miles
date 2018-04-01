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


bool Wal::log(const std::string& key, const std::vector<char>& payload) {
    this->file.clear();
    this->file << key << std::endl;
    this->file.write(reinterpret_cast<const char*>(&payload), sizeof(unsigned char));
    //this->file << payload.as_bytes() << std::endl;
    return true;
}

bool Wal::log(const std::string& key, const json& payload) {
    return Wal::log(key, utils::stringToCharVector(payload.dump()));
    //this->file.clear();
    //this->file << key << std::endl;
    //this->file.write(reinterpret_cast<const char*>(&payload), sizeof(unsigned char));
    //this->file << payload.as_bytes() << std::endl;
    //return true;
}


bool Wal::clear() {
    auto tmp = std::fstream(path, std::fstream::in | std::fstream::out | std::fstream::trunc);
    file = std::fstream(path, std::fstream::in | std::fstream::out | std::fstream::ate);
    return true;
}


std::pair<std::unique_ptr<Wal>, std::unique_ptr<KeyMap>> Wal::buildKeyMapAndWall(std::string walPath) {

    auto wal = std::make_unique<Wal>(walPath);
    auto keyMap = std::make_unique<KeyMap>();

    for(std::istream_iterator<std::string> it = wal->begin(); it != wal->end(); ++it) {
        auto key = *it;
        ++it;
        //auto payload =
        keyMap->store(key, json::parse(*it));
    }

   return std::make_pair(std::move(wal), std::move(keyMap));
}
