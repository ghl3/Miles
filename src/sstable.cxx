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
        file->write(reinterpret_cast<char*>(&idx.keyHash), sizeof(idx.keyHash));
        file->write(reinterpret_cast<char*>(&idx.offset), sizeof(idx.offset));
        file->write(reinterpret_cast<char*>(&idx.length), sizeof(idx.length));
    }

    *file << std::endl;

    return std::unique_ptr<SSTable>{new SSTable(fileName, std::move(file))};
}

std::vector<IndexEntry> SSTable::buildIndex() {

    uint64_t numKeys;

    file->seekg(0);
    file->read(reinterpret_cast<char *>(&numKeys), sizeof(uint64_t));

    // 3 uint64_t entries per index
    // each uint64_t is 8 bytes
    // there are numKeys entries
    uint64_t indexSize = 3 * sizeof(uint64_t) * numKeys;

    // Jump to the end of the file
    file->seekg(0, std::ios::end);
    long long int fileSize = file->tellg();
    uint64_t indexOffset = fileSize - indexSize;

    std::cout << " Num Keys: " << numKeys
              << " index size: " << indexSize
              << " file size: " << fileSize
              << " index offset: " << indexOffset
              << std::endl;

    // The index consists of the last 3*sizeof(size_t)*numKeys of the file;

    return std::vector<IndexEntry>();
}

/*
std::string SSTable::buildStringIndex(std::vector<IndexEntry>& index) {
    std::stringstream ss;
    return ss.str();
}
 */


/*
std::vector<IndexEntry> SSTable::buildInMemoryIndex(const KeyMap &km) {

    std::vector<std::pair<std::string, std::string>> data;

    for (auto keyValPair: km) {
        size_t keyHash = std::hash<std::string>{}(keyValPair.first);
        std::string compressedPayload = Zip::compress(keyValPair.second.dump());
        data.push_back(std::make_pair(keyHash, compressedPayload));
    }

    std::sort(begin(data), end(data));

    return std::unique_ptr<SSTable>{new SSTable(fileName, std::move(file))};


    return std::vector<IndexEntry>();
}

 \*/