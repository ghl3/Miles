#include <algorithm>

#include "sorter.h"
#include "gtest/gtest.h"


TEST(storage_test, map_storage)
{


    auto mapStorage = MapStorage();

    char arr[] = "abcdefghab";
    char eq[]  = "aabbcdefgh";
    size_t sz = sizeof(arr)/sizeof(arr[0]) - 1; // we need it, to avoid terminating \0 in "" definition case

    array_sort(arr, sz);

    for(int i=0; i<sz; i++)
	EXPECT_EQ(arr[i], eq[i]);
}

TEST(sorter_test, char_arr_sort)
{
    char arr[] = {'a','b','c','d','e','f','g','h','a','b'};
    char eq[]  = {'a','a','b','b','c','d','e','f','g','h'};
    size_t sz = sizeof(arr)/sizeof(arr[0]);

    array_sort(arr, sz);

    for(int i=0; i<sz; i++)
	EXPECT_EQ(arr[i], eq[i]);
}

TEST(sorter_test, int_arr_sort)
{
    int arr[] = {9,8,7,6,5,4,3,2,1,0};
    int eq[]  = {0,1,2,3,4,5,6,7,8,9};
    size_t sz = sizeof(arr)/sizeof(arr[0]);

    array_sort(arr, sz);

    for(int i=0; i<sz; i++)
	EXPECT_EQ(arr[i], eq[i]);
}
