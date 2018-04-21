//
// Created by George on 3/16/18.
//

#include <fstream>

#include <iostream>
#include <results.h>
#include <sstable.h>
#include <zip.h>

FetchResult SSTable::fetch(const std::string &key) {

    uint64_t keyHash = SSTable::hashKey(key);

    // TODO: Don't bring index into memory, use disk version directly
    std::vector<IndexEntry> index = this->buildIndex();

    // TODO: Do a faster search here
    for (IndexEntry idx : index) {
        if (idx.keyHash == keyHash) {
            return this->getData(idx);
        }
    }

    return FetchResult::error();
}

std::vector<IndexEntry> SSTable::buildIndex() {

    uint64_t numKeys;

    // Read the metadata at the beginning of the file
    file->seekg(0);
    file->read(reinterpret_cast<char *>(&numKeys), sizeof(uint64_t));

    // Find get the size and offset of the index
    file->seekg(0, std::ios::end);
    long long int fileSize = file->tellg();

    // 3 uint64_t entries per index
    // each uint64_t is 8 bytes
    // there are numKeys entries
    uint64_t indexSize = 3 * sizeof(uint64_t) * numKeys;
    uint64_t indexOffset = fileSize - indexSize;

    // Jump to the start of the index and read it off,
    // writing the results into a vector
    std::vector<IndexEntry> index(numKeys);
    file->seekg(indexOffset - 1, std::ios::beg);

    for (uint i = 0; i < numKeys; ++i) {

        // TODO: We can write the index values directly into an array
        // This will reduce copies when building the index
        uint64_t hash;
        file->read(reinterpret_cast<char *>(&hash), sizeof(uint64_t));

        uint64_t length;
        file->read(reinterpret_cast<char *>(&length), sizeof(uint64_t));

        uint64_t offset;

        file->read(reinterpret_cast<char *>(&offset), sizeof(uint64_t));

        index[i] = IndexEntry(hash, length, offset);
    }

    return index;
}

FetchResult SSTable::getData(IndexEntry idx) {
    file->seekg(idx.offset);
    std::string data(idx.length, '\0');
    file->read(&data[0], static_cast<std::streamsize>(idx.length));

    if (idx.compressed) {
        data = Zip::decompress(data);
    }

    return FetchResult::success(utils::stringToCharVector(data));
}

std::unique_ptr<SSTable>
SSTable::createFromFileName(const std::string &fileName) {

    // Create a new file
    auto file = std::make_unique<std::fstream>(
        fileName, std::fstream::in | std::fstream::out | std::fstream::ate);

    // Get the number of keys from the metadata at the beginning of the file
    Metadata metadata;
    file->seekg(0);
    file->read(reinterpret_cast<char *>(&metadata), sizeof(Metadata));

    return std::unique_ptr<SSTable>{
        new SSTable(fileName, std::move(file), metadata)};
}

uint64_t SSTable::hashKey(const std::string &key) {
    return std::hash<std::string>{}(key);
}

size_t COMPRESSION_THRESHOLD = 1024;

std::unique_ptr<SSTable> SSTable::createFromKeyMap(const Memtable &km,
                                                   std::string fileName) {

    // Create a new file
    auto file = std::make_unique<std::fstream>(
        fileName, std::fstream::in | std::fstream::out | std::fstream::trunc |
                      std::fstream::binary);

    // Start at the beginning of the file
    file->seekp(0);

    // Leave a blank space for the metadata
    // by skipping ahead the memory size
    file->seekp(sizeof(Metadata), file->beg);

    // Then, write each value to the file and maintain an index
    // for each entry
    uint64_t currentOffset = sizeof(Metadata);
    std::vector<IndexEntry> index;

    for (auto keyValPair : km) {

        uint64_t keyHash = SSTable::hashKey(keyValPair.first);

        std::string payload = utils::charVectorToString(keyValPair.second);

        bool compress = (payload.size() > COMPRESSION_THRESHOLD);
        if (compress) {
            payload = Zip::compress(payload);
        }

        uint64_t sizeInBytes = payload.size() * sizeof(char);
        index.emplace_back(
            IndexEntry(keyHash, currentOffset, sizeInBytes, compress));
        currentOffset += sizeInBytes;

        file->write(&payload[0], payload.size() * sizeof(char));
    }

    // Now that we've written all of the data, write the index
    // Mark the location of the start of the index
    auto indexOffset = static_cast<uint64_t>(file->tellg());

    std::sort(begin(index), end(index), [](const auto &lhs, const auto &rhs) {
        return lhs.keyHash < rhs.keyHash;
    });

    for (auto idx : index) {
        file->write(reinterpret_cast<char *>(&idx.keyHash),
                    sizeof(idx.keyHash));
        file->write(reinterpret_cast<char *>(&idx.offset), sizeof(idx.offset));
        file->write(reinterpret_cast<char *>(&idx.length), sizeof(idx.length));
    }

    *file << std::endl;

    // Finally, write the metadata to the beginning of the file
    auto metadata = Metadata(km.size(), indexOffset);
    file->seekp(0);
    file->write(reinterpret_cast<char *>(&metadata), sizeof(Metadata));

    return std::unique_ptr<SSTable>{
        new SSTable(fileName, std::move(file), metadata)};
}
