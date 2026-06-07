// string_test.cpp
// testing for string library code

#include <gtest/gtest.h>
#include <iostream>
#include "lib/string.h"

using thg::string;

TEST(StringBasicTest, StringConstructor)
{
	thg::string a;
	EXPECT_EQ(a.size(), 0);
}

