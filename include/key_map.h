//
// Created by George on 3/16/18.
//

#ifndef MILES_KEY_MAP_H
#define MILES_KEY_MAP_H

#include <string>
#include <unordered_map>
#include "results.h"
#include "key_storage.h"

class KeyMap: public IKeyStorage {

public:

    explicit KeyMap() {;}

    FetchResult fetch(std::string key) override;

    StoreResult store(std::string key, std::unique_ptr<json> payload);

    bool containsKey(const std::string& key) const;

    size_t size() {
        return data.size();
    };

    std::map<std::string, std::shared_ptr<json>>::const_iterator begin() const { return data.cbegin(); }
    std::map<std::string, std::shared_ptr<json>>::const_iterator end() const { return data.cend(); }

private:

    std::map<std::string, std::shared_ptr<json>> data;

};


#endif //MILES_KEY_MAP_H
