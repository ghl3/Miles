
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


TEST(hybrid_key_storage, store_and_fetch)
{

    utils::TempDirectory tmpDir("/tmp/miles/hybrid_key_storage_test_");

    auto storage = std::make_unique<Table>(tmpDir.getPath(), 10);
    EXPECT_EQ(false, storage->fetch("bar").isSuccess);

    auto payload = json::array({{"a", 10}, {"b", 20}});
    auto storeResult = storage->store("foo", payload);
    EXPECT_EQ(true, storeResult.isSuccess);

    EXPECT_EQ(true, storage->fetch("foo").isSuccess);
    EXPECT_EQ(json::array({{"a", 10}, {"b", 20}}), storage->fetch("foo").getPayload());
    EXPECT_EQ(false, storage->fetch("bar").isSuccess);

}


TEST(hybrid_key_storage, move_to_disk)
{

    utils::TempDirectory tmpDir("/tmp/miles/hybrid_key_storage_test_");

    auto storage = std::make_unique<Table>(tmpDir.getPath(), 2);
    EXPECT_EQ(false, storage->fetch("bar").isSuccess);

    storage->store("a", json::array({{"a", 10}, {"b", 20}}));
    storage->store("b", json::array({{"a", 10}, {"b", 20}}));
    storage->store("d", json::array({{"a", 10}, {"b", 20}}));
    storage->store("c", json::array({{"a", 10}, {"b", 20}}));
    storage->store("z", json::array({{"a", 10}, {"b", 20}}));

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


TEST(hybrid_key_storage, reload_from_file)
{

    utils::TempDirectory tmpDir("/tmp/miles/hybrid_key_storage_reload_from_file_");

    {
        auto storage = std::make_unique<Table>(tmpDir.getPath(), 2);
        storage->store("a", json::array({{"x", 10}, {"y", 20}}));
        storage->store("b", json::array({{"x", 20}, {"y", 40}}));
        storage->store("d", json::array({{"x", 30}, {"y", 60}}));
        storage->store("c", json::array({{"x", 40}, {"y", 80}}));
        storage->store("e", json::array({{"x", 50}, {"y", 100}}));
    }

    auto storage = Table::buildFromDirectory(tmpDir.getPath(), 2);

    EXPECT_EQ(json::array({{"x", 10}, {"y", 20}}), storage->fetch("a").getPayload());
    EXPECT_EQ(json::array({{"x", 20}, {"y", 40}}), storage->fetch("b").getPayload());

    auto CTHING = storage->fetch("c");

    EXPECT_EQ(json::array({{"x", 40}, {"y", 80}}), CTHING.getPayload());
    EXPECT_EQ(json::array({{"x", 30}, {"y", 60}}), storage->fetch("d").getPayload());
    EXPECT_EQ(json::array({{"x", 50}, {"y", 100}}), storage->fetch("e").getPayload());

}
