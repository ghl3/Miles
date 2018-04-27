//
// Created by George on 3/16/18.
//

#include <results.h>
#include <string>
#include <wal.h>

#include <fstream>
#include <iostream>

#include <constants.h>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <iterator>
#include <string>

Wal::Wal(const std::string& path) : path(path) {
    // Use a temporary stream to ensure the
    // file exists if it hasn't yet been created
    auto tmpFile = std::ofstream(path, std::ios::app);
    tmpFile.close();
    this->file = std::fstream(path, std::fstream::in | std::fstream::out | std::fstream::ate | std::fstream::binary);
};

/**
 * Truncate the WAL and reset it to a clean file
 * @return
 */
bool Wal::clear() {
    this->file.close();
    auto tmp = std::fstream(path, std::fstream::in | std::fstream::out | std::fstream::trunc);
    file = std::fstream(path, std::fstream::in | std::fstream::out | std::fstream::ate | std::fstream::binary);
    return true;
}

bool Wal::log(const std::string& key, const std::vector<char>& payload) {

    // Clear any error flags on the file
    // Note: This does NOT clear the contents of the file!
    this->file.clear();

    LogHeader header(0, key.size(), payload.size());
    this->file.write(reinterpret_cast<char*>(&header), sizeof(LogHeader));
    this->file.write(&(key[0]), key.size() * sizeof(char));
    this->file.write(&(payload[0]), payload.size() * sizeof(char));
    this->file.flush();

    return true;
}

bool Wal::log(const std::string& key, std::string&& payload) {
    // TODO: Optimize this
    return Wal::log(key, utils::stringToCharVector(payload));
}

bool Wal::del(const std::string& key) {

    // Clear any error flags on the file
    // Note: This does NOT clear the contents of the file!
    this->file.clear();

    LogHeader header(0, key.size(), 0);
    header.setDeleted();
    this->file.write(reinterpret_cast<char*>(&header), sizeof(LogHeader));
    this->file.write(&(key[0]), key.size() * sizeof(char));
    this->file.flush();

    return true;
}

void Wal::seekToBeginnig() { this->file.seekg(0); }

boost::optional<LogEntry> Wal::getNextEntry() {

    if (this->file.eof()) {
        return boost::none;
    }
    LogHeader header(0, 0, 0);
    this->file.read(reinterpret_cast<char*>(&header), sizeof(LogHeader));
    if (this->file.eof()) {
        LOG(INFO) << "Encountered EOF when reading WAL header. Discarding Entry." << std::endl;
        return boost::none;
    }

    if (header.getKeyLength() > constants::maxKeySize) {
        LOG(ERROR) << "Encountered bad key size. Discarding Entry." << std::endl;
        return boost::none;
    }

    if (header.getPayloadLength() > constants::maxPayloadSize) {
        LOG(ERROR) << "Encountered bad payload size. Discarding Entry." << std::endl;
        return boost::none;
    }

    std::string key;
    key.resize(static_cast<unsigned long>(header.getKeyLength()));
    this->file.read(&(key[0]), static_cast<std::streamsize>(header.getKeyLength()));
    if (this->file.eof()) {
        LOG(INFO) << "Encountered EOF when reading WAL key. Discarding Entry." << std::endl;
        return boost::none;
    }

    std::vector<char> payload(static_cast<unsigned long>(header.getPayloadLength()));
    this->file.read(&(payload[0]), static_cast<std::streamsize>(header.getPayloadLength()));
    if (this->file.eof()) {
        LOG(INFO) << "Encountered EOF when reading WAL val. Discarding Entry." << std::endl;
        return boost::none;
    }

    return LogEntry(header, std::move(key), std::move(payload));
}

Wal::KeyMapAndWal Wal::buildKeyMapAndWal(std::string walPath) {

    auto wal = std::make_unique<Wal>(walPath);
    auto keyMap = std::make_unique<Memtable>();

    wal->seekToBeginnig();
    while (true) {
        boost::optional<LogEntry> entry = wal->getNextEntry();
        if (entry.is_initialized()) {

            if (entry.get().isDelete()) {
                keyMap->del(entry.get().moveKey());

            } else {

                keyMap->store(entry.get().moveKey(), entry.get().movePayload());
            }

        } else {
            break;
        }
    }

    return std::make_pair(std::move(wal), std::move(keyMap));
}
