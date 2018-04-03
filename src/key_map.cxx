//
// Created by George on 3/16/18.
//

#include <memory>

#include <key_map.h>
#include <results.h>

FetchResult KeyMap::fetch(const std::string &key) {
  if (data.find(key) == data.end()) {
    return FetchResult::error();
  } else {
    // This copies the data and returns it to the user
    // This is the semantics we want (it allows the fetcher
    // to own the data)
    return FetchResult::success(std::vector<char>(data[key]));
  }
}

StoreResult KeyMap::store(const std::string &key, std::vector<char> &&payload) {
  // The data member takes ownership of the underlying payload data
  data.emplace(key, std::move(payload));
  return StoreResult(true);
}

bool KeyMap::containsKey(const std::string &key) const {
  return !(data.find(key) == data.end());
}
