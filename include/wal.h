//
// Created by George on 3/16/18.
//

#ifndef MILES_WAL_H
#define MILES_WAL_H

#include "constants.h"
#include "memtable.h"
#include "results.h"
#include <boost/optional.hpp>
#include <fstream>
#include <ostream>
#include <string>

class LogHeader {

  public:
    LogHeader(uint64_t metadata, uint64_t keyLength, uint64_t payloadLength)
        : metadata(metadata), keyLength(keyLength), payloadLength(payloadLength) {
        ;
    }

    uint64_t getKeyLength() { return this->keyLength; }
    uint64_t getPayloadLength() { return this->payloadLength; }

    void setDeleted() { metadata = metadata | constants::TOMBSTONE_MASK; }
    bool isDeleted() { return (metadata & constants::TOMBSTONE_MASK) != 0; }

  private:
    constants::Metadata metadata;

    uint64_t keyLength;

    uint64_t payloadLength;
};

class LogEntry {

  public:
    LogEntry(LogHeader header, std::string&& key, std::vector<char>&& payload)
        : header(header), key(std::move(key)), payload(std::move(payload)), isValid(true) {
        ;
    }

    LogEntry(LogEntry& rhs) = delete;
    LogEntry(const LogEntry& rhs) = delete;

    LogEntry(LogEntry&& rhs) noexcept
        : header(rhs.header), key(std::move(rhs.key)), payload(std::move(rhs.payload)), isValid(rhs.isValid) {
        ;
    }

    std::string&& moveKey() { return std::move(this->key); }
    std::vector<char>&& movePayload() { return std::move(this->payload); }

    bool isDelete() { return header.isDeleted(); }

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
    bool del(const std::string& key);

    bool clear();
    void seekToBeginnig();
    boost::optional<LogEntry> getNextEntry();

    static KeyMapAndWal buildKeyMapAndWal(std::string walPath);

  private:
    const std::string path;

    std::fstream file;
};

#endif // MILES_WAL_H
