
#include <utility>
#include <cstdlib>
#include <string>

#include "gtest/gtest.h"
#include <database.h>
#include <glog/logging.h>

#include "utils.h"

#include "json.h"
using json = nlohmann::json;


TEST(sstable, metadata) {
    Memtable keyMap;
    auto payload = json::array({{"a", 10}, {"b", 20}});
    keyMap.storeJson("foo", payload);

    utils::TempDirectory tmpDir("/tmp/miles/test/sstable_metadata_");
    std::string fname = (std::stringstream() << tmpDir.getPath() << "/" << "foobar" << ".dat").str();
    auto ssTable = SSTable::createFromKeyMap(keyMap, fname);

    auto metadata = ssTable->metadata;

    // Validate that the metadata makes sense
    // (There are other fields, but they may change,
    //  so we only change the ones here)
    EXPECT_EQ(1, metadata.getNumKeys());
    EXPECT_EQ(0, metadata.getHashSalt());
    EXPECT_LT(0, metadata.getIndexStart());
}


TEST(sstable, map_storage) {

    Memtable keyMap;

    auto payload = json::array({{"a", 10}, {"b", 20}});
    auto storeResult = keyMap.storeJson("foo", payload);
    EXPECT_EQ(true, storeResult.isSuccess);

    utils::TempDirectory tmpDir("/tmp/miles/test/sstable_map_storage_");
    std::string fname = (std::stringstream() << tmpDir.getPath() << "/" << "foobar" << ".dat").str();
    auto ssTable = SSTable::createFromKeyMap(keyMap, fname);

    auto x = ssTable->fetch("foo");

    EXPECT_EQ(true, ssTable->fetch("foo").isPresent);
    EXPECT_EQ(json::array({{"a", 10}, {"b", 20}}), ssTable->fetch("foo").getAsJson());
    EXPECT_EQ(false, ssTable->fetch("bar").isPresent);
}


TEST(sstable, many_keys) {

    Memtable keyMap;

    keyMap.storeJson("foo", json::array({{"a", 10}, {"b", 20},}));
    keyMap.storeJson("bar", json::array({{"a", 10}, {"b", 20},}));
    keyMap.storeJson("baz", json::array({{"a", 10}, {"b", 20},}));
    keyMap.storeJson("bat", json::array({{"a", 10}, {"b", 20},}));
    keyMap.storeJson("zap", json::array({{"a", 10}, {"b", 20},}));

    utils::TempDirectory tmpDir("/tmp/miles/test/sstable_many_keys_");
    std::string fname = (std::stringstream() << tmpDir.getPath() << "/" << "foobar" << ".dat").str();
    auto ssTable = SSTable::createFromKeyMap(keyMap, fname);

    EXPECT_EQ(true, ssTable->fetch("foo").isPresent);
    EXPECT_EQ(json::array({{"a", 10}, {"b", 20}}), ssTable->fetch("foo").getAsJson());
    EXPECT_EQ(true, ssTable->fetch("bar").isPresent);
    EXPECT_EQ(true, ssTable->fetch("baz").isPresent);
    EXPECT_EQ(true, ssTable->fetch("bat").isPresent);
    EXPECT_EQ(true, ssTable->fetch("zap").isPresent);

    EXPECT_EQ(false, ssTable->fetch("fish").isPresent);

}


/**
 * Test that we can create a SSTable from a memtable
 * and that the index is as we expect.
 * Further, test that the data stored also matches our expectations
 */
TEST(sstable, index) {

    Memtable keyMap;

    keyMap.storeJson("foo", json::array({{"a", 10}, {"b", 20},}));

    keyMap.storeJson("bar", json::array({{"a", 10}, {"b", 20},}));

    utils::TempDirectory tmpDir("/tmp/miles/test/sstable_index_");
    std::string fname = (std::stringstream() << tmpDir.getPath() << "/" << "foobar" << ".dat").str();
    auto ssTable = SSTable::createFromKeyMap(keyMap, fname);

    auto idx = ssTable->buildIndex();

    std::vector<std::pair<int, int>> expected = {
            std::make_pair(32, 19),
            std::make_pair(32+19, 19)
    };

    EXPECT_EQ(idx.size(), expected.size());
    for (uint i=0; i < idx.size(); ++i) {
        EXPECT_EQ(idx[i].getOffset(), expected[i].first);
        EXPECT_EQ(idx[i].getLength(), expected[i].second);
    }

    auto json = ssTable->getData(idx.at(0)).getAsJson();

    EXPECT_EQ(json::array({{"a", 10}, {"b", 20}}),  json);

}


TEST(sstable, del) {

    Memtable keyMap;

    keyMap.storeString("foo", "10");
    keyMap.storeString("bar", "10");
    keyMap.del("bar");

    utils::TempDirectory tmpDir("/tmp/miles/test/sstable_del_");
    std::string fname = (std::stringstream() << tmpDir.getPath() << "/" << "foobar" << ".dat").str();
    auto ssTable = SSTable::createFromKeyMap(keyMap, fname);

    EXPECT_EQ("10", ssTable->fetch("foo").getAsString());
    EXPECT_EQ(false, ssTable->fetch("bar").isPresent);
    EXPECT_EQ(ResultType::DELETED_IN_SSTABLE, ssTable->fetch("bar").resultType);

}