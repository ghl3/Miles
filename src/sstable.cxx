//
// Created by George on 3/16/18.
//


#include <results.h>
#include <sstable.h>

/*
StoreResult SSTable::store(std::string key, std::unique_ptr<json> payload) {

    //std::lock_guard<std::mutex> guard(this->diskMutex);

    if (this->tableMap->find(table) == this->tableMap->end()) {
        (*this->tableMap)[table] = std::fstream((std::stringstream() << directory << "/" << table << ".dat").str(),
                                                std::fstream::in | std::fstream::out | std::fstream::trunc);
    }

    (*this->tableMap)[table] << key << std::endl;
    (*this->tableMap)[table] << payload->dump() << std::endl;

    return StoreResult(true);
}
*/

FetchResult SSTable::fetch(std::string key) {

    file.seekg(0);
    std::string lineKey;
    std::string linePayload;
    while (std::getline(file, lineKey)) {

        std::getline(file, linePayload);

        if (lineKey == key) {
            return FetchResult(true, std::make_shared<json>(json::parse(linePayload)));
        }
    }

    return FetchResult(false);
}


/*
StoreResult SSTable::appendKey(std::string key, std::unique_ptr<json> payload) {
    if (this->tableMap->find(table) == this->tableMap->end()) {
        (*this->tableMap)[table] = std::fstream((std::stringstream() << directory << "/" << table << ".dat").str(),
                                                std::fstream::in | std::fstream::out | std::fstream::trunc);
    }

    (*this->tableMap)[table] << key << std::endl;
    (*this->tableMap)[table] << payload->dump() << std::endl;

    return StoreResult(true);}

*/

/*
SSTable SSTable::createFromKeyMap(KeyMap km, std::string fileName) {
    return SSTable(__1::basic_string < char, char_traits < char > , allocator < char >> (), __1::basic_fstream < char,
                   char_traits < char >> ());
}
*/

std::unique_ptr<SSTable> SSTable::createFromKeyMap(const KeyMap& km, std::string fileName) {

    // Create a new file
    auto file = std::fstream(fileName, std::fstream::in | std::fstream::out | std::fstream::trunc);

    for (auto keyValPair: km) {
        file << keyValPair.first << std::endl;
        file << keyValPair.second->dump() << std::endl;
    }

    return std::unique_ptr<SSTable>{new SSTable(fileName, std::move(file))};

}
