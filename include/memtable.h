//
// Created by George on 3/16/18.
//

#ifndef MILES_KEY_MAP_H
#define MILES_KEY_MAP_H

#include "fetchable.h"
#include "results.h"
#include "storable.h"
#include <set>
#include <string>
#include <unordered_map>

using json = nlohmann::json;

class Memtable : public IStorable, IFetchable {

  public:
    explicit Memtable() { ; }

    Memtable(const Memtable&) = delete;
    Memtable& operator=(const Memtable&) = delete;

    FetchResult fetch(const std::string& key) override;

    StoreResult store(const std::string& key, std::vector<char>&& payload) override;

    void del(const std::string& key) override;

    bool containsKey(const std::string& key) const;

    size_t numKeysPresentOrDeleted() const { return data.size() + deletedKeys.size(); };

    std::map<std::string, std::vector<char>>::const_iterator begin() const { return data.cbegin(); }
    std::map<std::string, std::vector<char>>::const_iterator end() const { return data.cend(); }

    const std::set<std::string>& getDeletedKeys() const { return deletedKeys; }

  private:
    std::map<std::string, std::vector<char>> data;

    std::set<std::string> deletedKeys;
};

#endif // MILES_KEY_MAP_H
