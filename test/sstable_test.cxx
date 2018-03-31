
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
    EXPECT_EQ(true, storeResult.isSuccess);

    TempDirectory tmpDir("/tmp/miles/ss_table_test_");

    std::string fname = (std::stringstream() << tmpDir.getPath() << "/" << "foobar" << ".dat").str();

    auto ssTable = SSTable::createFromKeyMap(*keyMap, fname);

    EXPECT_EQ(true, ssTable->fetch("foo").isSuccess);
    EXPECT_EQ(json::array({{"a", 10}, {"b", 20}}), ssTable->fetch("foo").getPayload());
    EXPECT_EQ(false, ssTable->fetch("bar").isSuccess);

}


TEST(sstable_test, many_keys)
{
    auto keyMap = std::make_unique<KeyMap>();

    keyMap->store("foo", std::make_unique<json>(
            json::array({{"a", 10}, {"b", 20},})));

    keyMap->store("bar", std::make_unique<json>(
            json::array({{"a", 10}, {"b", 20},})));

    keyMap->store("baz", std::make_unique<json>(
            json::array({{"a", 10}, {"b", 20},})));

    TempDirectory tmpDir("/tmp/miles/ss_table_test_");
    std::string fname = (std::stringstream() << tmpDir.getPath() << "/" << "foobar" << ".dat").str();

    auto ssTable = SSTable::createFromKeyMap(*keyMap, fname);

    EXPECT_EQ(true, ssTable->fetch("foo").isSuccess);
    EXPECT_EQ(json::array({{"a", 10}, {"b", 20}}), ssTable->fetch("foo").getPayload());
    EXPECT_EQ(true, ssTable->fetch("bar").isSuccess);
    EXPECT_EQ(false, ssTable->fetch("fish").isSuccess);

}




TEST(sstable_test, index_test)
{
    auto keyMap = std::make_unique<KeyMap>();

    keyMap->store("foo", std::make_unique<json>(
            json::array({{"a", 10}, {"b", 20},})));

    keyMap->store("bar", std::make_unique<json>(
            json::array({{"a", 10}, {"b", 20},})));

    TempDirectory tmpDir("/tmp/miles/ss_table_compressed_test_");
    std::string fname = (std::stringstream() << tmpDir.getPath() << "/" << "foobar" << ".dat").str();

    auto ssTable = SSTable::createCompressedFromKeyMap(*keyMap, fname);

    auto idx = ssTable->buildIndex();

    std::vector<IndexEntry> expected = {
            IndexEntry(658648847097844546, 8, 8),
            IndexEntry(910203208414753533, 16, 8)
    };

    EXPECT_EQ(idx.size(), expected.size());
    for (uint i=0; i < idx.size(); ++i) {
        EXPECT_EQ(idx[i], expected[i]);
    }

    std::cout << "Done" << std::endl;

}