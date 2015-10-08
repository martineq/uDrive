// test.cc

#include <iostream>
#include "test_basic.h"

using namespace std;

int main(int argc, char **argv) {
  
  TestBasic tb;
  tb.TestRocksDB();
  tb.TestMongoose();
  tb.TestJson();
  tb.TestDbHandler();
  tb.TestDataHandler();

  return 0;
}

