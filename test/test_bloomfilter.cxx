//
// Created by George Lewis on 4/21/18.
//


#include <bloomfilter.h>
#include "gtest/gtest.h"


TEST(bloomfilter, storage)
{

    auto bloomFilter = BloomFilter(10, 20);

    bloomFilter.add("foo");

    EXPECT_EQ(true, bloomFilter.containsKey("foo"));
    EXPECT_EQ(false, bloomFilter.containsKey("bar"));

}
