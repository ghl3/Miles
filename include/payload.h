

#ifndef MILES_PAYLOAD_H
#define MILES_PAYLOAD_H

#include <vector>
#include "utils.h"


class Payload {

public:

    explicit Payload(std::vector<char>&& data) noexcept: data(std::move(data)) {;}

    Payload(Payload&& other) noexcept {
        this->data.swap(other.data);
    }

    // A payload cannot be copied, only moved
    Payload(const Payload& that) = delete;
    Payload& operator=(const Payload&) = delete;
    Payload& operator=(const Payload&&) = delete;

    const std::string toString() {
        return utils::charVectorToString(this->data);
    }

    Payload makeCopy() {
        std::vector<char> newData(this->data.size());
        std::copy(this->data.begin(), this->data.end(),
                  std::back_inserter(newData));
        return Payload(std::move(newData));
    }

    static Payload fromString(const std::string& s) {
        return Payload(utils::stringToCharVector(s));
    }

    static Payload fromJson(const json& j) {
        return Payload(utils::jsonToCharVector(j));
    }


private:

    std::vector<char> data;

};



#endif //MILES_PAYLOAD_H
