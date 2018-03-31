//
// Created by George on 3/16/18.
//

#ifndef MILES_SSTABLE_H
#define MILES_SSTABLE_H

#include <fstream>
#include <utility>

#include "key_storage.h"
#include "key_map.h"

using json = nlohmann::json;


class IndexEntry {

public:

    explicit IndexEntry(uint64_t keyHash, uint64_t offset, uint64_t length, bool compressed=false):
            keyHash(keyHash),
            offset(offset),
            length(length),
            compressed(compressed)
    {;}

    // TODO: Hide this constructor for better encapsulation
    explicit IndexEntry(): keyHash(), offset(), length(), compressed(false) {;}

    bool operator==(const IndexEntry &other) const {
        return other.keyHash==this->keyHash
                && other.offset == this->offset
                && other.length == this->length;
    }

    friend class SSTable;

protected:

    uint64_t keyHash;
    uint64_t offset;
    uint64_t length;
    bool compressed;
};


class Metadata {

public:

    explicit Metadata(): numKeys(0), indexOffset(0) {;}

    explicit Metadata(uint64_t numKeys, uint64_t indexOffset): numKeys(numKeys), indexOffset(indexOffset) {;}

    inline uint64_t getDataStart() { return sizeof(Metadata); }
    inline uint64_t getIndexStart() { return indexOffset; }

    inline uint64_t getDataSize() { return getIndexStart() - getDataStart(); }
    inline uint64_t getIndexSize() { return 3 * sizeof(uint64_t) * numKeys; }

private:
    uint64_t numKeys;
    uint64_t indexOffset;
};


/**
 * A SSTable is an immutable, on-disk key-val table.
 * It supports the "fetch" protocol, allowing one to
 * fetch a JSON payload given a string key.
 *
 * The on-disk format of the table is the following:
 *
 * DATA
 * INDEX
 *
 * where the DATA section consists of gzipped JSON
 * payloads (stored as raw bytes) and the INDEX section
 * consists triplets:
 *
 * (keyHash, offset, length)
 *
 * with the following sizes:
 *
 * keyHash: 8 bytes
 * offset: 8 bytes
 * length: 8 bytes
 *
 * The index entries are ordered by their keyHash
 * (with the keyHash interpreted as an integer).
 * This allows for looking up the key in the index using
 * binary search (this is possible because all index entries
 * have the same size), and then one can fetch the
 * zipped data from the rest of the file using
 * the offset and length (unzipping and then converting
 * to JSON on the way out).
 *
 */
class SSTable: public IKeyStorage<json> {

public:

    FetchResult<json> fetch(std::string key) override;

    //FetchResult<IndexEntry> findInIndex(const std::string& key);

    std::vector<IndexEntry> buildIndex();

    static std::unique_ptr<SSTable> createFromKeyMap(const KeyMap& km, std::string fileName);

    //static std::unique_ptr<SSTable> createCompressedFromKeyMap(const KeyMap& km, std::string fileName);

    static std::unique_ptr<SSTable> createFromFileName(const std::string& fileName);

    std::unique_ptr<json> getData(IndexEntry idx);


private:

    //IndexEntry getIndexEntry(int i);

    // Hard-code metadata size to 8 bytes (1 int)
    // TODO: Make this dynamic, based on metadata size
    //inline uint64_t getIndexStart() { return 1 * sizeof(uint64_t); }
    //inline uint64_t getIndexSize() { return  3 * sizeof(uint64_t) * this; }
    //inline uint64_t getIndexEnd() { return getIndexStart() + getIndexSize(); }

    explicit SSTable(std::string fileName, std::unique_ptr<std::fstream> file, Metadata metadata) :
                     //uint64_t numEntries,  uint64_t dataOffset,  uint64_t indexOffset):
            fileName(std::move(fileName)),
            file(std::move(file)),
            metadata(metadata)
            {;}


    const std::string fileName;

    std::unique_ptr<std::fstream> file;

    const Metadata metadata;

    //const uint64_t numEntries;
    //const uint64_t dataOffset;
    //const uint64_t indexOffset;

};

#endif //MILES_SSTABLE_H
