
#include <utility>
#include <cstdlib>
#include <string>

#include "gtest/gtest.h"
#include <storage.h>

#include "utils.h"

#include "hybrid_key_storage.h"

#include "json.h"
using json = nlohmann::json;





TEST(hybrid_key_storage, map_storage)
{

TempDirectory tmpDir("/tmp/miles/hybrid_key_storage_test_");

auto storage = std::make_unique<HybridKeyStorage>(tmpDir.getPath(), 10);
EXPECT_EQ(false, storage->fetch("bar").success);

auto payload = std::make_unique<json>(json::array({{"a", 10}, {"b", 20}}));
auto storeResult = storage->store("foo", std::move(payload));
EXPECT_EQ(true, storeResult.success);

EXPECT_EQ(true, storage->fetch("foo").success);
EXPECT_EQ(json::array({{"a", 10}, {"b", 20}}), storage->fetch("foo").getJson());
EXPECT_EQ(false, storage->fetch("bar").success);


}
