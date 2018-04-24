//
// Created by George Lewis on 4/24/18.
//

#ifndef MILES_HASHING_H
#define MILES_HASHING_H

#include <boost/functional/hash.hpp>
#include <string>

namespace hashing {

uint64_t hashKeyWithSalt(const std::string& key, uint64_t salt) {

    if (salt > SIZE_MAX) {
        throw std::invalid_argument("Received a salt that is too big");
    }

    auto hashValue = static_cast<size_t>(salt);
    boost::hash_combine(hashValue, key);

    return hashValue;
}

} // namespace hashing

#endif // MILES_HASHING_H
