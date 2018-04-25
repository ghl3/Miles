//
// Created by George on 3/16/18.
//

#ifndef MILES_WAL_H
#define MILES_WAL_H

#include "memtable.h"
#include "results.h"
#include <fstream>
#include <ostream>
#include <string>

class Wal {
    using KeyMapAndWal = std::pair<std::unique_ptr<Wal>, std::unique_ptr<Memtable>>;

  public:
    explicit Wal(const std::string& path); // : path(path);
    bool log(const std::string& key, const std::vector<char>& payload);
    bool log(const std::string& key, std::string&& payload);

    bool clear();

    static KeyMapAndWal buildKeyMapAndWal(std::string walPath);

    // TODO: Make an iterator over lines in a WAL
    std::istream_iterator<std::string> begin() {
        // return begin(file);
        file.seekg(0);
        std::istream_iterator<std::string> in_iter(file);
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

#endif // MILES_WAL_H
