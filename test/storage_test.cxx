
#include <utility>
#include <cstdlib>
#include <string>

#include "gtest/gtest.h"
#include <storage.h>

#include "utils.h"

#include "json.h"
using json = nlohmann::json;


TEST(storage_test, map_storage)
{
    auto storage = std::make_unique<MapStorage>();
    EXPECT_EQ(false, storage->fetch("foo", "bar").success);

    auto payload = std::make_unique<json>(json::array({{"a", 10}, {"b", 20}}));
    auto storeResult = storage->store("foo", "bar", std::move(payload));
    EXPECT_EQ(true, storeResult.success);

    EXPECT_EQ(true, storage->fetch("foo", "bar").success);
    EXPECT_EQ(json::array({{"a", 10}, {"b", 20}}), storage->fetch("foo", "bar").getJson());

}



TEST(storage_test, disk_storage)
{

    TempDirectory tmpDir("/tmp/miles/storage_test_");

    auto storage = std::make_unique<DiskStorage>(tmpDir.getPath());
    EXPECT_EQ(false, storage->fetch("foo", "bar").success);

    auto payload = std::make_unique<json>(json::array({{"a", 10}, {"b", 20}}));
    auto storeResult = storage->store("foo", "bar", std::move(payload));
    EXPECT_EQ(true, storeResult.success);

    EXPECT_EQ(true, storage->fetch("foo", "bar").success);
    EXPECT_EQ(json::array({{"a", 10}, {"b", 20}}), storage->fetch("foo", "bar").getJson());

}



/*
TEST(storage_test, hybrid_storage)
{

    TempDirectory tmpDir("/tmp/miles/storage_test_");

    auto storage = std::make_unique<HybridStorage>(tmpDir.getPath());
    EXPECT_EQ(false, storage->fetch("foo", "bar").success);

    auto payload = std::make_unique<json>(json::array({{"a", 10}, {"b", 20}}));
    auto storeResult = storage->store("foo", "bar", std::move(payload));
    EXPECT_EQ(true, storeResult.success);

    EXPECT_EQ(true, storage->fetch("foo", "bar").success);
    EXPECT_EQ(json::array({{"a", 10}, {"b", 20}}), storage->fetch("foo", "bar").getJson());

}
*/