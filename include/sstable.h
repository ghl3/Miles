//
// Created by George on 3/16/18.
//

#ifndef MILES_SSTABLE_H
#define MILES_SSTABLE_H

#include <fstream>
#include <utility>

#include <boost/optional.hpp>

#include "memtable.h"
#include "storable.h"

#include "fetchable.h"
#include "gsl.h"
#include "constants.h"

class IndexEntry {



public:

    explicit IndexEntry() : keyHash(), offset(), length(), metadata() { ; }

    explicit IndexEntry(uint64_t keyHash,
                        uint64_t offset,
                        uint64_t length,
                        bool isCompressed=false,
                        bool isTombstone=false) : keyHash(keyHash), offset(offset), length(length), metadata(0) {
        if (isCompressed) {
            this->metadata = metadata | constants::COMPRESSION_MASK;
        }
        if (isTombstone) {
            this->metadata = metadata | constants::TOMBSTONE_MASK;
        }
    }

    uint64_t getKeyHash() const { return this->keyHash; }

    uint64_t getOffset() const { return this->offset; }

    uint64_t getLength() const { return this->length; }

    bool isCompressed() const { return (metadata & constants::COMPRESSION_MASK) != 0; }

    bool isTombstone() const { return (metadata & constants::TOMBSTONE_MASK) != 0; }


protected:
    // Hash of the key
    uint64_t keyHash;

    // Total file offset of start of data
    uint64_t offset;

    // Length of data
    uint64_t length;

    // Is data compressed
    constants::Metadata metadata;
};

class Metadata {

public:

    explicit Metadata(uint64_t numKeys, uint64_t indexOffset, uint64_t hashSalt, uint64_t compressionThreshold)
            : numKeys(numKeys), indexOffset(indexOffset), hashSalt(hashSalt), compressionThreshold(compressionThreshold) {
        ;
    }

    inline uint64_t getNumKeys() const { return numKeys; }

    inline uint64_t getDataStart() const { return sizeof(Metadata); }
    inline uint64_t getIndexStart() const { return indexOffset; }

    inline uint64_t getDataSize() const { return getIndexStart() - getDataStart(); }
    inline uint64_t getIndexSize() const { return 3 * sizeof(uint64_t) * numKeys; }

    inline uint64_t getHashSalt() const { return hashSalt; }

    static Metadata createFromFile(std::fstream* file) {
        Metadata metadata;
        file->read(reinterpret_cast<char*>(&metadata), sizeof(Metadata));
        return metadata;
    }

private:
    explicit Metadata() : numKeys(0), indexOffset(0), hashSalt(0), compressionThreshold(0) { ; }

    uint64_t numKeys;
    uint64_t indexOffset;
    uint64_t hashSalt;
    uint64_t compressionThreshold;
};

/**
 * A SSTable is an immutable, on-disk key-val table.
 * It supports the "fetch" protocol, allowing one to
 * fetch a JSON payload given a string key.
 *
 * The on-disk format of the table is the following:
 *
 * METADATA
 * DATA
 * INDEX
 *
 * where the DATA section consists of possibly-gzipped payloads
 * payloads (stored as raw bytes) and the INDEX section
 * consists triplets:
 *
 * (keyHash, offset, length, compressed)
 *
 * with the following sizes:
 *
 * keyHash: 8 bytes
 * offset: 8 bytes
 * length: 8 bytes
 * compressed: 1 byte
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
class SSTable : public IFetchable {

public:

    SSTable(const SSTable&)=delete;

    FetchResult fetch(const std::string& key) override;

    std::vector<IndexEntry> buildIndex();

    FetchResult getData(IndexEntry idx) const;

    static std::unique_ptr<SSTable> createFromKeyMap(const Memtable& km, std::string fileName,
                                                     uint64_t compressionThreshold = 1024);

    static std::unique_ptr<SSTable> createFromFileName(const std::string& fileName);

    const std::string fileName;

    const Metadata metadata;

private:
    explicit SSTable(std::string fileName, std::unique_ptr<std::fstream> file, Metadata metadata)
            : fileName(std::move(fileName)), metadata(metadata), file(std::move(file)) {
        ;
    }

    std::unique_ptr<std::fstream> file;

    /**
     * Hash the key to an 64 bit integer using the (salted)
     * hash function for this table.
     * @param key
     * @return
     */
    uint64_t hashKey(const std::string& key) const;

    /**
     * Fetch the ith index or none if this index does not exist.
     * @param idx
     * @return
     */
    boost::optional<IndexEntry> getIndexByIdx(uint64_t idx) const;

    /**
     * Fetch the index for the given key or none if this index does not exist.
     * @param idx
     * @return
     */
    boost::optional<IndexEntry> getIndexByKey(const std::string& key) const;
};

#endif // MILES_SSTABLE_H
