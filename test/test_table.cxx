
#include <utility>
#include <cstdlib>
#include <string>

#include "gtest/gtest.h"
#include <database.h>

#include "utils.h"

using json = nlohmann::json;


std::string fileToString(const std::string &fileName) {
    std::ifstream t(fileName);
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    return str;
}


TEST(table, store_and_fetch) {

    utils::TempDirectory tmpDir("/tmp/miles/test/table_store_and_fetch_");

    auto storage = std::make_unique<Table>(tmpDir.getPath(), 10);
    EXPECT_EQ(false, storage->fetch("bar").isPresent);

    auto payload = json::array({{"a", 10}, {"b", 20}});
    auto storeResult = storage->storeJson("foo", payload);
    EXPECT_EQ(true, storeResult.isSuccess);

    EXPECT_EQ(true, storage->fetch("foo").isPresent);
    EXPECT_EQ(json::array({{"a", 10}, {"b", 20}}), storage->fetch("foo").getAsJson());
    EXPECT_EQ(false, storage->fetch("bar").isPresent);

}


TEST(table, del) {

    utils::TempDirectory tmpDir("/tmp/miles/test/table_del_");

    Table table(tmpDir.getPath(), 2);

    table.storeString("foo", "10");
    EXPECT_EQ("10", table.fetch("foo").getAsString());

    // First, delete it and check that it was marked as deleted in the memtable
    table.del("foo");
    EXPECT_EQ(ResultType::DELETED_IN_MEMTABLE, table.fetch("foo").resultType);

    // Next, add more keys so that the memtable is moved to SSTable storage
    // and then check that we get the deletion from the SSTable
    table.storeString("bar", "20");
    table.storeString("baz", "30");
    //table.storeString("biff", "40");
    EXPECT_EQ(ResultType::DELETED_IN_SSTABLE, table.fetch("foo").resultType);

}


TEST(table, move_to_disk) {

    utils::TempDirectory tmpDir("/tmp/miles/test/table_move_to_disk_");

    auto storage = std::make_unique<Table>(tmpDir.getPath(), 2);
    EXPECT_EQ(false, storage->fetch("bar").isPresent);

    storage->storeJson("a", json::array({{"a", 10}, {"b", 20}}));
    storage->storeJson("b", json::array({{"a", 10}, {"b", 20}}));

    storage->storeJson("d", json::array({{"a", 10}, {"b", 20}}));
    storage->storeJson("c", json::array({{"a", 10}, {"b", 20}}));

    storage->storeJson("z", json::array({{"a", 10}, {"b", 20}}));

    EXPECT_EQ(false, storage->fetch("bar").isPresent);
    EXPECT_EQ(true, storage->fetch("a").isPresent);
    EXPECT_EQ(true, storage->fetch("z").isPresent);

    // Assert that we created 2 on-disk sstable files
    ASSERT_TRUE(boost::filesystem::exists(tmpDir.getPath() + "/table_0.dat"));
    ASSERT_TRUE(boost::filesystem::exists(tmpDir.getPath() + "/table_1.dat"));

    // Assert that we created a WAL and its current contents are what we expect
    std::string walPath = tmpDir.getPath() + "/wal.log";
    //std::string walFilePath = fileToString(tmpDir.getPath() + "/wal.log");

    auto walAndKeyMap = Wal::buildKeyMapAndWal(walPath);

    // 'z' is the only key in the wal, as the rest are persisted to dsik
    EXPECT_EQ(false, walAndKeyMap.second->containsKey("a"));
    EXPECT_EQ(false, walAndKeyMap.second->containsKey("b"));
    EXPECT_EQ(true, walAndKeyMap.second->containsKey("z"));

}


TEST(table, reload_from_file) {

    utils::TempDirectory tmpDir("/tmp/miles/test/table_reload_from_file_");

    {
        auto storage = std::make_unique<Table>(tmpDir.getPath(), 2);
        storage->storeJson("a", json::array({{"x", 10}, {"y", 20}}));
        storage->storeJson("b", json::array({{"x", 20}, {"y", 40}}));
        storage->storeJson("d", json::array({{"x", 30}, {"y", 60}}));
        storage->storeJson("c", json::array({{"x", 40}, {"y", 80}}));
        storage->storeJson("e", json::array({{"x", 50}, {"y", 100}}));
    }

    auto storage = Table::buildFromDirectory(tmpDir.getPath(), 2);

    EXPECT_EQ(json::array({{"x", 10}, {"y", 20}}), storage->fetch("a").getAsJson());
    EXPECT_EQ(json::array({{"x", 20}, {"y", 40}}), storage->fetch("b").getAsJson());
    EXPECT_EQ(json::array({{"x", 40}, {"y", 80}}), storage->fetch("c").getAsJson());
    EXPECT_EQ(json::array({{"x", 30}, {"y", 60}}), storage->fetch("d").getAsJson());
    EXPECT_EQ(json::array({{"x", 50}, {"y", 100}}), storage->fetch("e").getAsJson());

}
