//
// Created by George on 3/16/18.
//

#ifndef MILES_HYBRID_KEY_STORAGE_H
#define MILES_HYBRID_KEY_STORAGE_H


#include "key_storage.h"
#include "key_map.h"
#include "sstable.h"


class HybridKeyStorage: public IKeyStorage {

    // TODO: Make a HybridTableStorage, and then make the
    // storage engine have a list of HybridTableStorage objects
    // (one per table)

public:

    explicit HybridKeyStorage(std::string directory, size_t maxInMemorySize):
            maxInMemorySize(maxInMemorySize),
            directory(std::move(directory)) {;}

    FetchResult fetch(std::string key) override;

    StoreResult store(std::string key, std::unique_ptr<json> payload);


private:

    const size_t maxInMemorySize;

    std::unique_ptr<KeyMap> inMemoryStorage;

    std::vector<std::unique_ptr<SSTable>> diskStorage;

    const std::string directory;

    std::mutex lock;

};



#endif //MILES_HYBRID_KEY_STORAGE_H
