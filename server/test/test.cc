// test.cc

#include <iostream>
#include "test_basic.h"
#include "gtest/gtest.h"

using namespace std;

TEST(BasicTest, AlwaysOk) {

  TestBasic tb;
  tb.TestRocksDB();
  tb.TestMongoose();
  tb.TestJson();
  tb.TestDbHandler();
  tb.TestDataHandler();

  EXPECT_EQ(1,1);
}

int main(int argc, char **argv) {

  // Testing with GTest
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

