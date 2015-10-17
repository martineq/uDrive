// test.cc

#include <iostream>
#include <fstream>

#include "gtest/gtest.h"
#include "test_basic.h"
#include "../src/config_parser.h"

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

TEST(YamlTest, ReadsFromFile) {
  
  // Create config file
  ofstream myfile;
  myfile.open ("config.yml");
  myfile << "# Configuración de conexión\nip: 127.0.0.1\nport: 8080";
  myfile.close();
  
  // Read config file
  ConfigParser yp;
  ConfigParser::stConnection conn;
  conn.ip = "0";
  conn.port = "0";
  yp.load_configuration(conn);
  
  EXPECT_EQ(conn.ip,"127.0.0.1");
  EXPECT_EQ(conn.port,"8080");

}


int main(int argc, char **argv) {

  // Testing with GTest
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

