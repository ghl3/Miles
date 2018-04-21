
#include <utility>
#include <cstdlib>
#include <string>

#include "gtest/gtest.h"
#include <database.h>

#include "utils.h"

#include "json.h"
using json = nlohmann::json;




TEST(sstable_test, map_storage)
{
    auto keyMap = std::make_unique<Memtable>();
    auto payload = json::array({{"a", 10}, {"b", 20}});
    auto storeResult = keyMap->storeJson("foo", payload);
    EXPECT_EQ(true, storeResult.isSuccess);

    utils::TempDirectory tmpDir("/tmp/miles/ss_table_test_");

    std::string fname = (std::stringstream() << tmpDir.getPath() << "/" << "foobar" << ".dat").str();

    auto ssTable = SSTable::createFromKeyMap(*keyMap, fname);

    EXPECT_EQ(true, ssTable->fetch("foo").isSuccess);
    EXPECT_EQ(json::array({{"a", 10}, {"b", 20}}), ssTable->fetch("foo").getAsJson());
    EXPECT_EQ(false, ssTable->fetch("bar").isSuccess);

}


TEST(sstable_test, many_keys)
{
    auto keyMap = std::make_unique<Memtable>();

    keyMap->storeJson("foo", json::array({{"a", 10}, {"b", 20},}));

    keyMap->storeJson("bar", json::array({{"a", 10}, {"b", 20},}));

    keyMap->storeJson("baz", json::array({{"a", 10}, {"b", 20},}));

    utils::TempDirectory tmpDir("/tmp/miles/ss_table_test_");
    std::string fname = (std::stringstream() << tmpDir.getPath() << "/" << "foobar" << ".dat").str();

    auto ssTable = SSTable::createFromKeyMap(*keyMap, fname);

    EXPECT_EQ(true, ssTable->fetch("foo").isSuccess);
    EXPECT_EQ(json::array({{"a", 10}, {"b", 20}}), ssTable->fetch("foo").getAsJson());
    EXPECT_EQ(true, ssTable->fetch("bar").isSuccess);
    EXPECT_EQ(false, ssTable->fetch("fish").isSuccess);

}


TEST(sstable_test, index_test)
{
    auto keyMap = std::make_unique<Memtable>();

    keyMap->storeJson("foo", json::array({{"a", 10}, {"b", 20},}));

    keyMap->storeJson("bar", json::array({{"a", 10}, {"b", 20},}));

    utils::TempDirectory tmpDir("/tmp/miles/ss_table_compressed_test_");
    std::string fname = (std::stringstream() << tmpDir.getPath() << "/" << "foobar" << ".dat").str();

    auto ssTable = SSTable::createFromKeyMap(*keyMap, fname);

    auto idx = ssTable->buildIndex();

    std::vector<IndexEntry> expected = {
            IndexEntry(658648847097844546, 24, 19),
            IndexEntry(910203208414753533, 43, 19)
    };

    EXPECT_EQ(idx.size(), expected.size());
    for (uint i=0; i < idx.size(); ++i) {
        EXPECT_EQ(idx[i], expected[i]);
    }

    auto json = ssTable->getData(idx.at(0)).getAsJson();

    EXPECT_EQ(json::array({{"a", 10}, {"b", 20}}),  json);

}