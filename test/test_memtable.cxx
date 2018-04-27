
#include <utility>
#include <cstdlib>
#include <string>

#include "gtest/gtest.h"
#include <database.h>

#include "utils.h"

using json = nlohmann::json;


TEST(memtable, store) {

    Memtable storage;
    EXPECT_EQ(false, storage.fetch("bar").isSuccess);

    json payload = json::array({{"a", 10}, {"b", 20}});
    auto storeResult = storage.storeJson("foo", payload);
    EXPECT_EQ(true, storeResult.isSuccess);

    EXPECT_EQ(true, storage.fetch("foo").isSuccess);

    auto fetched = storage.fetch("foo");
    auto fetchedJson = fetched.getAsJson();

    EXPECT_EQ(json::array({{"a", 10}, {"b", 20}}), fetchedJson);
    EXPECT_EQ(false, storage.fetch("bar").isSuccess);

}


TEST(memtable, del) {

    Memtable storage;

    storage.storeString("foo", "10");
    storage.storeString("bar", "10");
    storage.del("bar");

    EXPECT_EQ("10", storage.fetch("foo").getAsString());
    EXPECT_EQ(false, storage.fetch("bar").isSuccess);

}
