
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


TEST(table, store_and_fetch)
{

    utils::TempDirectory tmpDir("/tmp/miles/hybrid_key_storage_test_");

    auto storage = std::make_unique<Table>(tmpDir.getPath(), 10);
    EXPECT_EQ(false, storage->fetch("bar").isSuccess);

    auto payload = json::array({{"a", 10}, {"b", 20}});
    auto storeResult = storage->storeJson("foo", payload);
    EXPECT_EQ(true, storeResult.isSuccess);

    EXPECT_EQ(true, storage->fetch("foo").isSuccess);
    EXPECT_EQ(json::array({{"a", 10}, {"b", 20}}), storage->fetch("foo").getAsJson());
    EXPECT_EQ(false, storage->fetch("bar").isSuccess);

}


TEST(table, move_to_disk)
{

    utils::TempDirectory tmpDir("/tmp/miles/hybrid_key_storage_test_");

    auto storage = std::make_unique<Table>(tmpDir.getPath(), 2);
    EXPECT_EQ(false, storage->fetch("bar").isSuccess);

    storage->storeJson("a", json::array({{"a", 10}, {"b", 20}}));
    storage->storeJson("b", json::array({{"a", 10}, {"b", 20}}));
    storage->storeJson("d", json::array({{"a", 10}, {"b", 20}}));
    storage->storeJson("c", json::array({{"a", 10}, {"b", 20}}));
    storage->storeJson("z", json::array({{"a", 10}, {"b", 20}}));

    EXPECT_EQ(false, storage->fetch("bar").isSuccess);
    EXPECT_EQ(true, storage->fetch("a").isSuccess);
    EXPECT_EQ(true, storage->fetch("z").isSuccess);

    // Assert that we created 2 on-disk sstable files
    ASSERT_TRUE(boost::filesystem::exists(tmpDir.getPath() + "/table_0.dat"));
    ASSERT_TRUE(boost::filesystem::exists(tmpDir.getPath() + "/table_1.dat"));

    // Assert that we created a WAL and its current contents are what we expect
    std::string walFile = fileToString(tmpDir.getPath() + "/wal.log");
    EXPECT_EQ(walFile, "z\n"
                       "[[\"a\",10],[\"b\",20]]\n");

}


TEST(table, reload_from_file)
{

    utils::TempDirectory tmpDir("/tmp/miles/hybrid_key_storage_reload_from_file_");

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
