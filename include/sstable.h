//
// Created by George on 3/16/18.
//

#ifndef MILES_SSTABLE_H
#define MILES_SSTABLE_H

#include <fstream>
#include <utility>

#include "key_storage.h"
#include "key_map.h"


class IndexEntry {

public:

    explicit IndexEntry(uint64_t keyHash, uint64_t offset, uint64_t length):
            keyHash(keyHash),
            offset(offset),
            length(length)
    {;}

    // TODO: Hide this constructor for better encapsulation
    explicit IndexEntry(): keyHash(), offset(), length(), payload() {;}

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
    std::string payload;

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
class SSTable: public IKeyStorage {

public:

    FetchResult fetch(std::string key) override;

    std::vector<IndexEntry> buildIndex();

    static std::unique_ptr<SSTable> createFromKeyMap(const KeyMap& km, std::string fileName);

    static std::unique_ptr<SSTable> createCompressedFromKeyMap(const KeyMap& km, std::string fileName);

    static std::unique_ptr<SSTable> createFromFileName(std::string fileName);

private:

    explicit SSTable(std::string fileName, std::unique_ptr<std::fstream> file):
            fileName(std::move(fileName)),
            file(std::move(file)) {;}

    const std::string fileName;

    std::unique_ptr<std::fstream> file;

};

#endif //MILES_SSTABLE_H
