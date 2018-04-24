//
// Created by George Lewis on 4/21/18.
//

#include "bloomfilter.h"
#include <boost/functional/hash.hpp>

BloomFilter::BloomFilter(size_t numBits, size_t numHashes) : bitMap(numBits, false), hashSeeds(numHashes) {
    for (size_t i = 0; i < numHashes; ++i) {
        hashSeeds.at(i) = i;
    }
}

std::vector<bool> BloomFilter::getHashBits(const std::string& key) {

    std::vector<bool> hashBits(bitMap.size());

    for (unsigned long bitIdx : hashSeeds) {
        boost::hash_combine(bitIdx, key);
        hashBits[bitIdx % bitMap.size()] = true;
    }

    return hashBits;
}

void BloomFilter::add(const std::string& key) {

    std::vector<bool> hashBits = getHashBits(key);

    for (size_t i = 0; i < bitMap.size(); ++i) {
        if (hashBits[i]) {
            bitMap.at(i) = true;
        }
    }
}

bool BloomFilter::containsKey(const std::string& key) {

    std::vector<bool> hashBits = getHashBits(key);

    for (size_t i = 0; i < bitMap.size(); ++i) {
        // If the bit is set for THIS key but not
        // in the bit map, then we KNOW FOR SURE
        // that this key has not been added.
        if (hashBits[i] && !bitMap[i]) {
            return false;
        }
    }

    // If all the bits for this key are set to on,
    // then we return true (either this key is present
    // or it's a false-positive)
    return true;
}

void BloomFilter::clear() {
    for (auto&& i : bitMap) {
        i = false;
    }
}
