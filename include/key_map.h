//
// Created by George on 3/16/18.
//

#ifndef MILES_KEY_MAP_H
#define MILES_KEY_MAP_H

#include "fetchable.h"
#include "results.h"
#include "storable.h"
#include <string>
#include <unordered_map>

using json = nlohmann::json;

class KeyMap : public IStorable, IFetchable {

  public:
    explicit KeyMap() { ; }

    FetchResult fetch(const std::string &key) override;

    StoreResult store(const std::string &key,
                      std::vector<char> &&payload) override;

    bool containsKey(const std::string &key) const;

    size_t size() const { return data.size(); };

    std::map<std::string, std::vector<char>>::const_iterator begin() const {
        return data.cbegin();
    }
    std::map<std::string, std::vector<char>>::const_iterator end() const {
        return data.cend();
    }

  private:
    std::map<std::string, std::vector<char>> data;
};

#endif // MILES_KEY_MAP_H
