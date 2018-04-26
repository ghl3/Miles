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
#include <boost/optional.hpp>


class LogHeader {

public:
    LogHeader(uint8_t metadata, uint64_t keyLength, uint64_t payloadLength) :
    metadata(metadata),
    keyLength(keyLength),
    payloadLength(payloadLength) {;}

    // TODO: can we mak this smaller?
    uint64_t metadata;

    uint64_t keyLength;

    uint64_t payloadLength;
};


class LogEntry {

public:
    LogEntry(LogHeader header, std::string&& key, std::vector<char>&& payload):
            header(header),
            key(std::move(key)),
            payload(std::move(payload)),
            isValid(true) {;}

    LogEntry(LogEntry& rhs)=delete;
    LogEntry(const LogEntry& rhs)=delete;

    LogEntry(LogEntry&& rhs) noexcept: header(rhs.header),
                                       key(std::move(rhs.key)),
                                       payload(std::move(rhs.payload)),
                                       isValid(rhs.isValid) {;}

    std::string&& moveKey() { return std::move(this->key);}
    std::vector<char>&& movePayload() { return std::move(this->payload);}

private:

    LogHeader header;

    std::string key;

    std::vector<char> payload;

    bool isValid;
};

/**
 * A Write-Ahead-Log
 * The data is stored in a custom binary format, consisting of:
 * - A LogHeader (defined above)
 * - The key as a string of chars
 * - The payload as a string of chars
 */
class Wal {
    using KeyMapAndWal = std::pair<std::unique_ptr<Wal>, std::unique_ptr<Memtable>>;

public:

    Wal(const Wal&) = delete;

    explicit Wal(const std::string& path);
    bool log(const std::string& key, const std::vector<char>& payload);
    bool log(const std::string& key, std::string&& payload);
    //bool del(const std::string& key);

    bool clear();
    void seekToBeginnig();
    boost::optional<LogEntry> getNextEntry();

    static KeyMapAndWal buildKeyMapAndWal(std::string walPath);

/*
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


    std::fstream& getFile() { return file; }
    */

private:
    const std::string path;

    std::fstream file;
};

#endif // MILES_WAL_H
