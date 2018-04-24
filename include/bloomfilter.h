//
// Created by George Lewis on 4/21/18.
//

#ifndef MILES_BLOOMFILTER_H
#define MILES_BLOOMFILTER_H

#include <bitset>
#include <string>
#include <unordered_map>
#include <vector>

/**
 *
 * A BloomFilter is a probabilistic set (key lookup) data structure.
 *
 * It has the following properties:
 * - If the bloom filter reports that an element IS NOT present, then the key is
 *   certainly not present
 * - If the bloom filter reports that an eleemnt IS present, then the key
 *   may be present, but it may not be.
 *
 * A BloomFilter works in the following way:
 * - It contains N bits
 * - It uses M hash functions, each of which creates a hash of space size N
 * - Every time an element is added, it is hashed M times (each being an integer
 *   between 0 and N-1)
 *
 */
class BloomFilter {

  public:
    BloomFilter(size_t numBits, size_t numHashes);

    void add(const std::string& key);

    bool containsKey(const std::string& key);

    void clear();

  private:
    std::vector<bool> getHashBits(const std::string& key);

    std::vector<bool> bitMap;

    std::vector<size_t> hashSeeds;
};

#endif // MILES_BLOOMFILTER_H
