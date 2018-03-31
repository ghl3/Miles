//
// Created by George on 3/16/18.
//

#ifndef MILES_KEY_MAP_H
#define MILES_KEY_MAP_H

#include <string>
#include <unordered_map>
#include "results.h"
#include "key_storage.h"

using json = nlohmann::json;


class KeyMap: public IKeyStorage<json> {

public:

    explicit KeyMap() {;}

    FetchResult<json> fetch(std::string key) override;

    StoreResult store(std::string key, std::unique_ptr<json> payload);

    bool containsKey(const std::string& key) const;

    size_t size() const {
        return data.size();
    };

    std::map<std::string, std::shared_ptr<json>>::const_iterator begin() const { return data.cbegin(); }
    std::map<std::string, std::shared_ptr<json>>::const_iterator end() const { return data.cend(); }

private:

    std::map<std::string, std::shared_ptr<json>> data;

};


#endif //MILES_KEY_MAP_H
