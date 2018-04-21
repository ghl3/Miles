
#include <utility>
#include <cstdlib>
#include <string>

#include "gtest/gtest.h"
#include <database.h>

#include "utils.h"

using json = nlohmann::json;


TEST(memtable, map_storage)
{

    auto storage = std::make_unique<Memtable>();
    EXPECT_EQ(false, storage->fetch("bar").isSuccess);

    json payload = json::array({{"a", 10}, {"b", 20}});
    auto storeResult = storage->storeJson("foo", payload);
    EXPECT_EQ(true, storeResult.isSuccess);

    EXPECT_EQ(true, storage->fetch("foo").isSuccess);

    auto fetched = storage->fetch("foo");
    auto fetchedJson = fetched.getAsJson();

    EXPECT_EQ(json::array({{"a", 10}, {"b", 20}}), fetchedJson);
    EXPECT_EQ(false, storage->fetch("bar").isSuccess);


}
