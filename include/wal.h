//
// Created by George on 3/16/18.
//

#ifndef MILES_WAL_H
#define MILES_WAL_H

#include <ostream>
#include <fstream>
#include <string>
#include "results.h"
#include "key_map.h"

class Wal {

public:

    explicit Wal(const std::string &path) : path(path) {
        // Use a temporary stream to ensure hte
        // file exists if it hasn't yet been created
        auto tmpFile = std::ofstream(path, std::ios::app);
        tmpFile.close();
        file = std::fstream(path, std::fstream::in | std::fstream::out | std::fstream::ate);
    }


    bool log(std::string key, const json& payload);

    bool clear();

    static std::pair<std::unique_ptr<Wal>, std::unique_ptr<KeyMap>>  buildKeyMapAndWall(std::string walPath);

    // TODO: Make an iterator over lines in a WAL
    std::istream_iterator<std::string> begin() {
        //return begin(file);
        file.seekg(0);
        std::istream_iterator<std::string> in_iter(file) ;
        return in_iter;
    }

    std::istream_iterator<std::string> end() {
        std::istream_iterator<std::string> eof;
        return eof;
    }


private:

    const std::string path;

    std::fstream file;

};

#endif //MILES_WAL_H
