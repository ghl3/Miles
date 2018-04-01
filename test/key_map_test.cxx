
#include <utility>
#include <cstdlib>
#include <string>

#include "gtest/gtest.h"
#include <database.h>

#include "utils.h"

using json = nlohmann::json;


TEST(key_map_test, map_storage)
{

    auto storage = std::make_unique<KeyMap>();
    EXPECT_EQ(false, storage->fetch("bar").isSuccess);

    auto payload = json::array({{"a", 10}, {"b", 20}});
    auto storeResult = storage->store("foo", payload);
    EXPECT_EQ(true, storeResult.isSuccess);

    EXPECT_EQ(true, storage->fetch("foo").isSuccess);
    EXPECT_EQ(json::array({{"a", 10}, {"b", 20}}), storage->fetch("foo").getPayload());
    EXPECT_EQ(false, storage->fetch("bar").isSuccess);


}
