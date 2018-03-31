
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

    TempDirectory tmpDir("/tmp/miles/storage_test_");

    auto storage = std::make_unique<Storage>(tmpDir.getPath(), 10);
    EXPECT_EQ(false, storage->fetch("foo", "bar").isSuccess);

    auto payload = std::make_unique<json>(json::array({{"a", 10}, {"b", 20}}));
    auto storeResult = storage->store("foo", "bar", std::move(payload));
    EXPECT_EQ(true, storeResult.isSuccess);

    EXPECT_EQ(true, storage->fetch("foo", "bar").isSuccess);
    EXPECT_EQ(json::array({{"a", 10}, {"b", 20}}), storage->fetch("foo", "bar").getPayload());

}
