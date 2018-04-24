//
// Created by George on 3/13/18.
//

#include <random>
#include <string>
#include <utils.h>

std::string utils::generateRandomId(size_t length) {

    static const std::string allowed_chars{"123456789abcdefghjklmnpqrstuvwxyz"};

    static thread_local std::default_random_engine randomEngine(std::random_device{}());
    static thread_local std::uniform_int_distribution<size_t> randomDistribution(0, allowed_chars.size() - 1);

    std::string id(length ? length : 32, '\0');

    for (std::string::value_type& c : id) {
        c = allowed_chars[randomDistribution(randomEngine)];
    }

    return id;
}

std::vector<char> utils::stringToCharVector(const std::string& s) {
    std::vector<char> retval;
    std::copy(s.begin(), s.end(), std::back_inserter(retval));
    return retval;
}

std::string utils::charVectorToString(const std::vector<char>& data) {
    std::string result(data.begin(), data.end());
    return result;
}

std::vector<char> utils::jsonToCharVector(const json& data) {
    return utils::stringToCharVector(data.dump());
    // std::string result(data.begin(), data.end());
    // return result;
}
