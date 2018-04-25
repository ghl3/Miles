
#include <utility>
#include <cstdlib>
#include <string>

#include "gtest/gtest.h"
#include <database.h>

#include "utils.h"

using json = nlohmann::json;


TEST(wal, write_read) {

    utils::TempDirectory tmpDir("/tmp/miles/wal_test_");

    std::string walPath = tmpDir.getPath() + "/wal.log";

    // Create a WAL file and write data to it
    auto wal = Wal(walPath);
    wal.log("foo", "a=>1");
    wal.log("bar", "b=>2");

    // Convert the WAL file to an in-memory map
    // and ensure the data is consistent
    auto walAndKeyMap = Wal::buildKeyMapAndWal(walPath);
    auto keyMap = std::move(walAndKeyMap.second);

    EXPECT_EQ("a=>1", keyMap->fetch("foo").getAsString());
    EXPECT_EQ("b=>2", keyMap->fetch("bar").getAsString());
    EXPECT_EQ(false, keyMap->fetch("baz").isSuccess);

}


TEST(wal, file_layout) {

    utils::TempDirectory tmpDir("/tmp/miles/wal_test_");

    std::string walPath = tmpDir.getPath() + "/wal.log";

    // Create a WAL file and write data to it
    auto wal = Wal(walPath);
    wal.log("foo", "a=>1");
    wal.log("bar", "b=>2");

    std::string expectedWal = "foo\n"
                              "a=>1\n"
                              "bar\n"
                              "b=>2\n"
                              ;

    {
        std::ifstream walFile(walPath);
        std::string walContents((std::istreambuf_iterator<char>(walFile)), std::istreambuf_iterator<char>());
        EXPECT_EQ(expectedWal, walContents);
    }

    wal.clear();

    {
        std::ifstream walFile(walPath);
        std::string walContents((std::istreambuf_iterator<char>(walFile)), std::istreambuf_iterator<char>());
        EXPECT_EQ("", walContents);
    }
}
