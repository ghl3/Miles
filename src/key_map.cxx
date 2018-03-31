//
// Created by George on 3/16/18.
//


#include <memory>

#include <results.h>
#include <key_map.h>


FetchResult<json> KeyMap::fetch(std::string key) {
    if(data.find(key) == data.end()) {
        return FetchResult<json>::error();
    } else {
        return FetchResult<json>::success(std::make_unique<json>(*data[key]));
    }
}


StoreResult KeyMap::store(std::string key, std::unique_ptr<json> payload) {

    // The underlying storage takes ownership of the JSON
    data[key] = std::move(payload);

    return StoreResult(true);
}


bool KeyMap::containsKey(const std::string& key) const {
    return !(data.find(key) == data.end());
}

