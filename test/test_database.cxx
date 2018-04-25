
#include <utility>
#include <cstdlib>
#include <string>

#include "gtest/gtest.h"
#include <database.h>

#include "utils.h"

#include "json.h"
using json = nlohmann::json;


TEST(storage, map_storage) {

    utils::TempDirectory tmpDir("/tmp/miles/storage_test_");

    auto storage = std::make_unique<Database>(tmpDir.getPath(), 10);
    EXPECT_EQ(false, storage->fetch("foo", "bar").isSuccess);

    auto payload = json::array({{"a", 10}, {"b", 20}});
    auto storeResult = storage->storeJson("foo", "bar", payload);
    EXPECT_EQ(true, storeResult.isSuccess);

    EXPECT_EQ(true, storage->fetch("foo", "bar").isSuccess);
    EXPECT_EQ(json::array({{"a", 10}, {"b", 20}}), storage->fetch("foo", "bar").getAsJson());

}
