
#include <utility>
#include <cstdlib>
#include <string>

#include "gtest/gtest.h"
#include <storage.h>

#include "utils.h"

#include "hybrid_key_storage.h"

#include "json.h"
using json = nlohmann::json;

#include <string>
#include <fstream>
#include <streambuf>


std::string fileToString(const std::string &fileName) {
    std::ifstream t(fileName);
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    return str;
}


TEST(hybrid_key_storage, store_and_fetch)
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


TEST(hybrid_key_storage, move_to_disk)
{

    TempDirectory tmpDir("/tmp/miles/hybrid_key_storage_test_");

    auto storage = std::make_unique<HybridKeyStorage>(tmpDir.getPath(), 2);
    EXPECT_EQ(false, storage->fetch("bar").success);

    storage->store("a", std::make_unique<json>(json::array({{"a", 10}, {"b", 20}})));
    storage->store("b", std::make_unique<json>(json::array({{"a", 10}, {"b", 20}})));
    storage->store("d", std::make_unique<json>(json::array({{"a", 10}, {"b", 20}})));
    storage->store("c", std::make_unique<json>(json::array({{"a", 10}, {"b", 20}})));
    storage->store("z", std::make_unique<json>(json::array({{"a", 10}, {"b", 20}})));

    EXPECT_EQ(false, storage->fetch("bar").success);

    std::string dataFile0 = fileToString(tmpDir.getPath() + "/table_0.dat");
    EXPECT_EQ(dataFile0, "a\n"
                         "[[\"a\",10],[\"b\",20]]\n"
                         "b\n"
                         "[[\"a\",10],[\"b\",20]]\n");

    std::string dataFile1 = fileToString(tmpDir.getPath() + "/table_1.dat");
    EXPECT_EQ(dataFile1, "c\n"
                         "[[\"a\",10],[\"b\",20]]\n"
                         "d\n"
                         "[[\"a\",10],[\"b\",20]]\n");

    std::string walFile = fileToString(tmpDir.getPath() + "/wal.log");
    EXPECT_EQ(walFile, "a\n"
                         "[[\"a\",10],[\"b\",20]]\n"
                         "b\n"
                         "[[\"a\",10],[\"b\",20]]\n"
                         "d\n"
                         "[[\"a\",10],[\"b\",20]]\n"
                         "c\n"
                         "[[\"a\",10],[\"b\",20]]\n"
                         "z\n"
                         "[[\"a\",10],[\"b\",20]]\n");

}




TEST(hybrid_key_storage, reload_from_file)
{

    TempDirectory tmpDir("/tmp/miles/hybrid_key_storage_reload_from_file_");

    {
        auto storage = std::make_unique<HybridKeyStorage>(tmpDir.getPath(), 2);
        storage->store("a", std::make_unique<json>(json::array({{"a", 10}, {"b", 20}})));
        storage->store("b", std::make_unique<json>(json::array({{"a", 10}, {"b", 20}})));
        storage->store("d", std::make_unique<json>(json::array({{"a", 10}, {"b", 20}})));
        storage->store("c", std::make_unique<json>(json::array({{"a", 10}, {"b", 20}})));
        storage->store("z", std::make_unique<json>(json::array({{"a", 10}, {"b", 20}})));
    }

    auto storage = HybridKeyStorage::buildFromDirectory(tmpDir.getPath(), 2);

    EXPECT_EQ(json::array({{"a", 10}, {"b", 20}}), storage->fetch("a").getJson());

}
