
#include <utility>
#include <cstdlib>
#include <string>

#include "gtest/gtest.h"
#include <database.h>

#include "utils.h"

using json = nlohmann::json;


TEST(wal_test, build)
{

    utils::TempDirectory tmpDir("/tmp/miles/wal_test_");

    std::string walPath = tmpDir.getPath() + "/wal.log";

    // Create a WAL file and write data to it
    auto wal = Wal(walPath);
    wal.logJson("foo", json({{"a", 10}}));
    wal.logJson("bar", json({{"b", 20}}));

    // Convert the WAL file to an in-memory map
    // and ensure the data is consistent
    auto walAndKeyMap = Wal::buildKeyMapAndWall(walPath);
    auto keyMap = std::move(walAndKeyMap.second);

    EXPECT_EQ(json({{"a", 10}}), keyMap->fetch("foo").getAsJson());
}
