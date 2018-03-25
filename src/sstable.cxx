//
// Created by George on 3/16/18.
//

#include <fstream>

#include <results.h>
#include <sstable.h>
#include <zip.h>


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

    std::vector<IndexEntry> index;

    size_t offsetInBytes = 0;

    for (auto keyValPair: km) {

        size_t keyHash = std::hash<std::string>{}(keyValPair.first);
        const char* compressedPayload = Zip::compress(keyValPair.second->dump()).c_str();
        //std::string compressedPayload = keyValPair.second->dump();

        size_t sizeInBytes = sizeof(compressedPayload) ; //compressedPayload.size() * sizeof(char);
        index.emplace_back(IndexEntry(keyHash, offsetInBytes, sizeInBytes));
        offsetInBytes += sizeInBytes;

        file->write(reinterpret_cast<char*>(&compressedPayload), sizeof(compressedPayload));

        //*file << compressedPayload;
    }

    std::sort(begin(index), end(index), [](const auto& lhs, const auto& rhs) {return lhs.keyHash < rhs.keyHash;});

    for (auto idx: index) {
        file->write(reinterpret_cast<char*>(&idx.keyHash), sizeof(idx.keyHash));
        file->write(reinterpret_cast<char*>(&idx.offset), sizeof(idx.offset));
        file->write(reinterpret_cast<char*>(&idx.length), sizeof(idx.length));
        //*file << idx.keyHash << '|';
        //*file << idx.offset << '|';
        //*file << idx.length << '|';
    }

    *file << std::endl;

    return std::unique_ptr<SSTable>{new SSTable(fileName, std::move(file))};
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