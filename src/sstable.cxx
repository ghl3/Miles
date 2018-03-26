//
// Created by George on 3/16/18.
//

#include <fstream>

#include <results.h>
#include <sstable.h>
#include <zip.h>
#include <iostream>


FetchResult SSTable::fetch(std::string key) {

    this->file->clear();
    this->file->seekg(0);
    std::string lineKey;
    std::string linePayload;
    while (std::getline(*file, lineKey)) {

        std::getline(*file, linePayload);

        if (lineKey == key) {
            auto fr = FetchResult::success(std::make_shared<json>(json::parse(linePayload)));
            return fr;
        }
    }

    return FetchResult::error();
}


std::unique_ptr<SSTable> SSTable::createFromKeyMap(const KeyMap& km, std::string fileName) {

    // Create a new file
    auto file = std::make_unique<std::fstream>(fileName, std::fstream::in | std::fstream::out | std::fstream::trunc);

    for (auto keyValPair: km) {
        *file << keyValPair.first << std::endl;
        *file << keyValPair.second->dump() << std::endl;
    }

    return std::unique_ptr<SSTable>{new SSTable(fileName, std::move(file))};

}


std::unique_ptr<SSTable> SSTable::createFromFileName(std::string fileName) {

    // Create a new file
    auto file = std::make_unique<std::fstream>(fileName, std::fstream::in | std::fstream::out | std::fstream::ate);

    return std::unique_ptr<SSTable>{new SSTable(fileName, std::move(file))};
}


std::unique_ptr<SSTable> SSTable::createCompressedFromKeyMap(const KeyMap& km, std::string fileName) {

    // Create a new file
    auto file = std::make_unique<std::fstream>(fileName, std::fstream::in | std::fstream::out | std::fstream::trunc | std::fstream::binary);

    // Add the header/metadata to the file
    // For now, this only consists of an initial integer
    // that represents the number of bits
    uint64_t numKeys = static_cast<uint64_t>(km.size());
    file->write(reinterpret_cast<char*>(&numKeys), sizeof(uint64_t));

    // Then, write each value to the file and maintain an index
    // for each entry
    uint64_t offsetInBytes = sizeof(numKeys);
    std::vector<IndexEntry> index;

    for (auto keyValPair: km) {

        uint64_t keyHash = std::hash<std::string>{}(keyValPair.first);
        const char* compressedPayload = Zip::compress(keyValPair.second->dump()).c_str();

        uint64_t sizeInBytes = sizeof(compressedPayload) ; //compressedPayload.size() * sizeof(char);
        index.emplace_back(IndexEntry(keyHash, offsetInBytes, sizeInBytes));
        offsetInBytes += sizeInBytes;

        file->write(reinterpret_cast<char*>(&compressedPayload), sizeof(compressedPayload));
    }

    std::sort(begin(index), end(index), [](const auto& lhs, const auto& rhs) {return lhs.keyHash < rhs.keyHash;});

    for (auto idx: index) {

        std::cout << "Writing"
                  << " hash: " << idx.keyHash
                  << " offset " << idx.offset
                  << " length " << idx.length
                                << std::endl;

        file->write(reinterpret_cast<char*>(&idx.keyHash), sizeof(idx.keyHash));
        file->write(reinterpret_cast<char*>(&idx.offset), sizeof(idx.offset));
        file->write(reinterpret_cast<char*>(&idx.length), sizeof(idx.length));
    }

    *file << std::endl;

    return std::unique_ptr<SSTable>{new SSTable(fileName, std::move(file))};
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
    file->seekg(indexOffset-1, std::ios::beg);

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
