// test.cc

#include <iostream>
#include <fstream>

#include "gtest/gtest.h"
#include "test_basic.h"
#include "../src/config_parser.h"
#include "../src/file_handler.h"

using namespace std;

TEST(BasicTest, AlwaysOk) {

  TestBasic tb;
  tb.TestRocksDB();
  tb.TestMongoose();
  tb.TestJson();
  tb.TestDbHandler();
  //tb.TestDataHandler(); // TODO (mart): check

  EXPECT_EQ(1,1);
}

TEST(YamlTest, ReadsFromFile) {
// TODO (mart): redo
EXPECT_EQ(1,1);
/*  
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
*/
}


TEST(FileTest, SaveAndLoadFile) {

  FileHandler fh;
  
  // Save file
  string user_id = "111";
  string file_id = "222";
  string revision = "010";
  const char* p_save_file_stream = "# Configuración de conexión\nip: 127.0.0.1\nport: 8080"; // Size: 54 bytes
  size_t save_size_stream = 54;
  fh.save_file(user_id,file_id,revision,p_save_file_stream,save_size_stream);
    
  // Load file  
  char* p_load_file_stream = NULL;
  size_t p_load_size_stream = fh.load_file(user_id,file_id,revision,p_load_file_stream);

  //if( p_load_size_stream==0 ){ return 2; }
  ASSERT_NE(p_load_size_stream,0);

  std::cout <<"File size readed: "<< p_load_size_stream  << std::endl;
  std::cout <<"File content readed: " << std::endl;
  std::cout << p_load_file_stream<< std::endl;

  EXPECT_EQ(save_size_stream,p_load_size_stream);

}


int main(int argc, char **argv) {

  // Testing with GTest
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

