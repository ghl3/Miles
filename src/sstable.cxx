//
// Created by George on 3/16/18.
//

#include <fstream>

#include <hashing.h>
#include <iostream>
#include <results.h>
#include <sstable.h>
#include <zip.h>

#include "results.h"

FetchResult SSTable::fetch(const std::string& key) {

    boost::optional<IndexEntry> idxOpt = getIndexByKey(key);

    if (idxOpt.is_initialized()) {
        return this->getData(idxOpt.get());
    } else {
        return FetchResult::error(ResultType::FOUND_IN_SSTABLE);
    }
}

FetchResult SSTable::getData(IndexEntry idx) const {
    file->seekg(idx.getOffset());
    std::string data(idx.getLength(), '\0');
    file->read(&data[0], static_cast<std::streamsize>(idx.getLength()));

    if (idx.isCompressed()) {
        data = Zip::decompress(data);
    }

    return FetchResult::success(utils::stringToCharVector(data), ResultType::FOUND_IN_SSTABLE);
}

boost::optional<IndexEntry> SSTable::getIndexByIdx(uint64_t idx) const {
    if (idx > this->metadata.getNumKeys()) {
        return boost::none;
    } else {
        // Find the offset,
        // load the memory,
        // cast to an IndexEntry,
        // and return

        uint64_t memoryOffset = this->metadata.getIndexStart() + idx * sizeof(IndexEntry);

        IndexEntry indexEntry;
        file->seekg(memoryOffset);
        file->read(reinterpret_cast<char*>(&indexEntry), sizeof(IndexEntry));
        return indexEntry;
    }
}

boost::optional<IndexEntry> SSTable::getIndexByKey(const std::string& key) const {

    const uint64_t keyHash = this->hashKey(key);

    // Find the start and the end of the index
    uint64_t leftIdx = 0;
    uint64_t rightIdx = this->metadata.getNumKeys() - 1;

    // Check the boundaries
    boost::optional<IndexEntry> left = this->getIndexByIdx(leftIdx);
    if (left.is_initialized() && left.get().getKeyHash() == keyHash) {
        return left.get();
    }

    boost::optional<IndexEntry> right = this->getIndexByIdx(rightIdx);
    if (right.is_initialized() && right.get().getKeyHash() == keyHash) {
        return right.get();
    }

    // Perform binary search
    while (rightIdx > leftIdx + 1) {

        uint64_t middleIdx = leftIdx + (rightIdx - leftIdx) / 2;
        boost::optional<IndexEntry> middle = this->getIndexByIdx(middleIdx);
        if (middle.is_initialized() && middle.get().getKeyHash() == keyHash) {
            return middle.get();
        } else if (keyHash < middle.get().getKeyHash()) {
            rightIdx = middleIdx;
        } else {
            leftIdx = middleIdx;
        }
    }

    return boost::none;
}

std::vector<IndexEntry> SSTable::buildIndex() {

    file->seekg(0);
    Metadata metadata = Metadata::createFromFile(file.get());

    // Find get the size and offset of the index
    file->seekg(0, std::ios::end);

    // Jump to the start of the index and read it off,
    // writing the results into a vector
    std::vector<IndexEntry> index(metadata.getNumKeys());
    file->seekg(metadata.getIndexStart(), std::ios::beg);

    for (uint i = 0; i < metadata.getNumKeys(); ++i) {
        IndexEntry ie;
        file->read(reinterpret_cast<char*>(&ie), sizeof(IndexEntry));
        // TODO: We can write the index values directly into an array
        index[i] = ie;
    }

    return index;
}

std::unique_ptr<SSTable> SSTable::createFromFileName(const std::string& fileName) {

    // Create a new file
    auto file = std::make_unique<std::fstream>(fileName, std::fstream::in | std::fstream::out | std::fstream::ate);

    // Get the number of keys from the metadata at the beginning of the file
    // Metadata metadata;
    file->seekg(0);
    Metadata metadata = Metadata::createFromFile(file.get());

    return std::unique_ptr<SSTable>{new SSTable(fileName, std::move(file), metadata)};
}

uint64_t SSTable::hashKey(const std::string& key) const {
    return hashing::hashKeyWithSalt(key, this->metadata.getHashSalt());
}

std::unique_ptr<SSTable> SSTable::createFromKeyMap(const Memtable& km, std::string fileName,
                                                   uint64_t compressionThreshold) {

    // Create a new file
    auto file = std::make_unique<std::fstream>(fileName, std::fstream::in | std::fstream::out | std::fstream::trunc |
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

    uint64_t hashSalt = 0;

    for (auto keyValPair : km) {

        // TODO: Update salt and re-hash if there are any hash collisions
        uint64_t keyHash = hashing::hashKeyWithSalt(keyValPair.first, hashSalt);

        // TODO: Avoid conversions to-and-from char vector here
        std::string payload = utils::charVectorToString(keyValPair.second);

        bool compress = (payload.size() > compressionThreshold);
        if (compress) {
            payload = Zip::compress(payload);
        }

        size_t sizeInBytes = payload.size() * sizeof(char);
        index.emplace_back(keyHash, currentOffset, sizeInBytes, compress);
        currentOffset += sizeInBytes;

        // TODO: Check parentheses
        file->write(&(payload[0]), sizeInBytes);
    }

    std::sort(begin(index), end(index),
              [](const auto& lhs, const auto& rhs) { return lhs.getKeyHash() < rhs.getKeyHash(); });

    for (auto idx : index) {
        file->write(reinterpret_cast<char*>(&idx), sizeof(IndexEntry));
    }

    *file << std::endl;

    // Finally, write the metadata to the beginning of the file
    auto metadata = Metadata(km.size(), currentOffset, hashSalt, compressionThreshold);
    file->seekp(0);
    file->write(reinterpret_cast<char*>(&metadata), sizeof(Metadata));
    file->flush();

    return std::unique_ptr<SSTable>{new SSTable(fileName, std::move(file), metadata)};
}
