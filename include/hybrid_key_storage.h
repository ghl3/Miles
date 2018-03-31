//
// Created by George on 3/16/18.
//

#ifndef MILES_HYBRID_KEY_STORAGE_H
#define MILES_HYBRID_KEY_STORAGE_H

#include <vector>

#include "key_storage.h"
#include "key_map.h"
#include "sstable.h"
#include "wal.h"


// A HybridKeyStorage is a storage engine that combines
// on-disk SS-Tables and an in-memory key-val map
// (which is backed by a Write-Ahead-Log for durability).
// New writes are written to the in-memory storage and the WAL.
//
// When the in-memory storage reaches a maximum size, it is
// copied to make a new SSTable on-disk, and it and the WAL
// are then cleared (the WAL should always be in-sync with
// the in-memory storage).
//
class HybridKeyStorage: public IKeyStorage<json> {

    // TODO: Make a HybridTableStorage, and then make the
    // storage engine have a list of HybridTableStorage objects
    // (one per table)

public:

    explicit HybridKeyStorage(std::string directory, size_t maxInMemorySize):
            directory(std::move(directory)),
            maxInMemorySize(maxInMemorySize),
            inMemoryStorage(std::make_unique<KeyMap>()),
            wal(std::make_unique<Wal>((std::stringstream() << this->directory << "/wal.log").str()))
           {;}

    ~HybridKeyStorage() override;

    FetchResult<json> fetch(std::string key) override;

    StoreResult store(std::string key, std::unique_ptr<json> payload);

    static std::vector<std::string>  getDataFiles(std::string directory);

    static std::unique_ptr<HybridKeyStorage> buildFromDirectory(std::string directory, size_t maxInMemorySize);

private:

    bool saveInMemoryToDisk();

    const std::string directory;

    const size_t maxInMemorySize;

    std::unique_ptr<KeyMap> inMemoryStorage;

    std::vector<std::unique_ptr<SSTable>> diskStorage;

    std::unique_ptr<Wal> wal;

    std::mutex lock;

};

#endif //MILES_HYBRID_KEY_STORAGE_H
