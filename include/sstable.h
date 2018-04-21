//
// Created by George on 3/16/18.
//

#ifndef MILES_SSTABLE_H
#define MILES_SSTABLE_H

#include <fstream>
#include <utility>

#include "memtable.h"
#include "storable.h"

#include "fetchable.h"
#include "gsl.h"

class IndexEntry {

  public:
    explicit IndexEntry(uint64_t keyHash, uint64_t offset, uint64_t length,
                        bool compressed = false)
        : keyHash(keyHash), offset(offset), length(length),
          compressed(compressed) {
        ;
    }

    // TODO: Hide this constructor for better encapsulation
    explicit IndexEntry() : keyHash(), offset(), length(), compressed(false) {
        ;
    }

    bool operator==(const IndexEntry &other) const {
        return other.keyHash == this->keyHash && other.offset == this->offset &&
               other.length == this->length;
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
    explicit Metadata() : numKeys(0), indexOffset(0), hashSalt(0) { ; }

    explicit Metadata(uint64_t numKeys, uint64_t indexOffset,
                      uint64_t hashSalt = 0)
        : numKeys(numKeys), indexOffset(indexOffset), hashSalt(hashSalt) {
        ;
    }

    inline uint64_t getDataStart() { return sizeof(Metadata); }
    inline uint64_t getIndexStart() { return indexOffset; }

    inline uint64_t getDataSize() { return getIndexStart() - getDataStart(); }
    inline uint64_t getIndexSize() { return 3 * sizeof(uint64_t) * numKeys; }

  private:
    uint64_t numKeys;
    uint64_t indexOffset;

    // TODO: Use this to avoid hash collisions within a single SSTable file
    uint64_t hashSalt;
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
 * where the DATA section consists of gzipped JSON
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
    FetchResult fetch(const std::string &key) override;

    static std::unique_ptr<SSTable> createFromKeyMap(const Memtable &km,
                                                     std::string fileName);

    static std::unique_ptr<SSTable>
    createFromFileName(const std::string &fileName);

    std::vector<IndexEntry> buildIndex();

    FetchResult getData(IndexEntry idx);

  private:
    explicit SSTable(std::string fileName, std::unique_ptr<std::fstream> file,
                     Metadata metadata)
        : fileName(std::move(fileName)), file(std::move(file)),
          metadata(metadata) {
        ;
    }

    static uint64_t hashKey(const std::string &key);

    const std::string fileName;

    std::unique_ptr<std::fstream> file;

    const Metadata metadata;
};

#endif // MILES_SSTABLE_H
