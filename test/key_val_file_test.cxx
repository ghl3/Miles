//
// Created by George on 3/13/18.
//


#include <utility>
#include <cstdlib>
#include <string>
#include <sstream>

#include "gtest/gtest.h"
#include <storage.h>

#include "utils.h"

#include "key_val_file.h"

#include "json.h"
using json = nlohmann::json;



TEST(storage_test, key_val_file_test)
{

    TempDirectory tmpDir("/tmp/miles/key_val_file_test_");

    auto fileName = (std::stringstream() << tmpDir.getPath() << "/" << "keyVal.dat").str();

    auto storage = std::make_unique<KeyValFile>(fileName);
    EXPECT_EQ(false, storage->fetch("foo").success);

    auto payload = std::make_unique<json>(json::array({{"a", 10}, {"b", 20}}));
    auto storeResult = storage->store("foo", std::move(payload));
    EXPECT_EQ(true, storeResult.success);

    EXPECT_EQ(true, storage->fetch("foo").success);
    EXPECT_EQ(json::array({{"a", 10}, {"b", 20}}), storage->fetch("foo").getJson());

}


