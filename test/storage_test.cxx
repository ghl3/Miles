
#include <utility>

#include <utility>
#include <string>

#include <algorithm>
#include "gtest/gtest.h"

#include <storage.h>

#include "json.h"
using json = nlohmann::json;


TEST(storage_test, map_storage)
{
    auto mapStorage = MapStorage();
    EXPECT_EQ(false, mapStorage.fetch("foo", "bar").result);

    auto payload = std::make_unique<json>(json::array({{"a", 10}, {"b", 20}}));
    auto storeResult = mapStorage.store("foo", "bar", std::move(payload));
    EXPECT_EQ(true, storeResult.result);

    EXPECT_EQ(true, mapStorage.fetch("foo", "bar").result);
    EXPECT_EQ(json::array({{"a", 10}, {"b", 20}}), mapStorage.fetch("foo", "bar").getJson());

}
