//
// Created by George on 3/16/18.
//

#ifndef MILES_HYBRID_KEY_STORAGE_H
#define MILES_HYBRID_KEY_STORAGE_H

#include <vector>

#include "bloomfilter.h"
#include "memtable.h"
#include "sstable.h"
#include "storable.h"
#include "wal.h"

// A Table is a storage engine that combines
// on-disk SS-Tables and an in-memory key-val map
// (which is backed by a Write-Ahead-Log for durability).
// New writes are written to the in-memory storage and the WAL.
//
// When the in-memory storage reaches a maximum size, it is
// copied to make a new SSTable on-disk, and it and the WAL
// are then cleared (the WAL should always be in-sync with
// the in-memory storage).
//
class Table : public IStorable, IFetchable {

  public:
    explicit Table(std::string directory, size_t maxInMemorySize)
        : directory(std::move(directory)), maxInMemorySize(maxInMemorySize),
          inMemoryStorage(std::make_unique<Memtable>()),
          wal(std::make_unique<Wal>((std::stringstream() << this->directory << "/wal.log").str())),
          bloomFilter(1000, 50) {
        ;
    }

    ~Table() override;

    Table(const Table&) = delete;
    Table& operator=(const Table&) = delete;

    FetchResult fetch(const std::string& key) override;

    StoreResult store(const std::string& key, std::vector<char>&& payload) override;

    void del(const std::string& key) override;

    static std::vector<std::string> getDataFiles(std::string directory);

    static std::unique_ptr<Table> buildFromDirectory(std::string directory, size_t maxInMemorySize);

  private:
    bool saveInMemoryToDisk();

    const std::string directory;

    const size_t maxInMemorySize;

    std::unique_ptr<Memtable> inMemoryStorage;

    std::vector<std::unique_ptr<SSTable>> diskStorage;

    std::unique_ptr<Wal> wal;

    BloomFilter bloomFilter;

    std::mutex lock;
};

#endif // MILES_HYBRID_KEY_STORAGE_H
