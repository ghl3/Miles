//
// Created by George on 3/16/18.
//


#include <string>
#include <results.h>
#include <wal.h>


bool Wal::log(std::string key, const json* payload) {
    this->file << key << std::endl;
    this->file << payload->dump() << std::endl;

    return true;
}


bool Wal::clear() {
    return true;
}
