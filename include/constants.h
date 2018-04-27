//
// Created by George Lewis on 4/27/18.
//

#ifndef MILES_CONSTANTS_H
#define MILES_CONSTANTS_H


#include <cstdint>

namespace constants {

    const static uint64_t maxKeySize = 1024;

    const static uint64_t maxPayloadSize = 1024;

    // TODO: Can we make this smaller?
    typedef uint64_t Metadata;

    const static uint64_t COMPRESSION_MASK = 1u << 0u;

    const static uint64_t TOMBSTONE_MASK = 1u << 1u;

}

#endif //MILES_CONSTANTS_H
