
#include <utility>
#include <cstdlib>
#include <string>

#include "gtest/gtest.h"
#include <storage.h>

#include "utils.h"

#include "json.h"
using json = nlohmann::json;



TEST(sstable_test, map_storage)
{

  auto keyMap = std::make_unique<KeyMap>();
auto payload = std::make_unique<json>(json::array({{"a", 10}, {"b", 20}}));
auto storeResult = keyMap->store("foo", std::move(payload));
EXPECT_EQ(true, storeResult.success);


TempDirectory tmpDir("/tmp/miles/ss_table_test_");

std::string fname = (std::stringstream() << tmpDir.getPath() << "/" << "foobar" << ".dat").str();


auto ssTable = SSTable::createFromKeyMap(*keyMap, fname);


EXPECT_EQ(true, ssTable->fetch("foo").success);
EXPECT_EQ(json::array({{"a", 10}, {"b", 20}}), ssTable->fetch("foo").getJson());
EXPECT_EQ(false, ssTable->fetch("bar").success);



//auto storage = std::make_unique<SSTable>();
//EXPECT_EQ(false, storage->fetch("bar").success);




/*
auto payload = std::make_unique<json>(json::array({{"a", 10}, {"b", 20}}));
auto storeResult = storage->store("foo", std::move(payload));
EXPECT_EQ(true, storeResult.success);

*/

}
