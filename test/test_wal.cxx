
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
    Wal wal(walPath);
    wal.log("foo", "a=>1");
    wal.log("bar", "b=>2");

    // Convert the WAL file to an in-memory map
    // and ensure the data is consistent
    auto walAndKeyMap = Wal::buildKeyMapAndWal(walPath);
    auto& keyMap = walAndKeyMap.second;

    EXPECT_EQ("a=>1", keyMap->fetch("foo").getAsString());
    EXPECT_EQ("b=>2", keyMap->fetch("bar").getAsString());
    EXPECT_EQ(false, keyMap->fetch("baz").isPresent);

}



TEST(wal, del) {

    utils::TempDirectory tmpDir("/tmp/miles/wal_test_");

    std::string walPath = tmpDir.getPath() + "/wal.log";

    // Create a WAL file and write data to it
    Wal wal(walPath);
    wal.log("foo", "a=>1");
    wal.log("bar", "b=>2");
    wal.del("bar");

    // Convert the WAL file to an in-memory map
    // and ensure the data is consistent
    auto walAndKeyMap = Wal::buildKeyMapAndWal(walPath);
    auto& keyMap = walAndKeyMap.second;

    EXPECT_EQ("a=>1", keyMap->fetch("foo").getAsString());
    EXPECT_EQ(false, keyMap->fetch("bar").isPresent);
    EXPECT_EQ(false, keyMap->fetch("baz").isPresent);

}



TEST(wal, currupt_key) {


    utils::TempDirectory tmpDir("/tmp/miles/wal_currupt_key_");

    std::string walPath = tmpDir.getPath() + "/wal.log";

    // Encode the binary representation of 0(64b) 1(64b) 1(64b) 'k' 'v' (litle endian)
    std::ofstream out(walPath, std::fstream::binary);
    out.write("\0\0\0\0\0\0\0\0\1\0\0\0\0\0\0\0\1\0\0\0\0\0\0\0kv", 26);

    // Encode the binary representation of 0(64b) 1(64b) 1(64b) 'z' (litle endian)
    // This is invalid because the key has only 1 character (and there is no value)
    out.write("\0\0\0\0\0\0\0\0\2\0\0\0\0\0\0\0\1\0\0\0\0\0\0\0z", 25);

    out.close();

    auto walAndKeyMap = Wal::buildKeyMapAndWal(walPath);

    EXPECT_EQ(true, walAndKeyMap.second->containsKey("k"));
    EXPECT_EQ(false, walAndKeyMap.second->containsKey("z"));

}


TEST(wal, currupt_val) {

    utils::TempDirectory tmpDir("/tmp/miles/wal_currupt_val_");

    std::string walPath = tmpDir.getPath() + "/wal.log";

    // Encode the binary representation of 0(64b) 1(64b) 1(64b) 'k' 'v' (litle endian)
    std::ofstream out(walPath, std::fstream::binary);
    out.write("\0\0\0\0\0\0\0\0\1\0\0\0\0\0\0\0\1\0\0\0\0\0\0\0kv", 26);

    // Encode the binary representation of 0(64b) 1(64b) 1(64b) 'z' (litle endian)
    // This is invalid because it doesn't include a value (only 25 chars written)
    out.write("\0\0\0\0\0\0\0\0\1\0\0\0\0\0\0\0\1\0\0\0\0\0\0\0z", 25);

    out.close();

    auto walAndKeyMap = Wal::buildKeyMapAndWal(walPath);

    EXPECT_EQ(true, walAndKeyMap.second->containsKey("k"));
    EXPECT_EQ(false, walAndKeyMap.second->containsKey("z"));

}
