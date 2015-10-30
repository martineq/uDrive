// test.cc

#include <iostream>
#include <fstream>

#include "gtest/gtest.h"
#include "test_basic.h"
#include "../src/config_parser.h"
#include "../src/file_handler.h"
#include "../src/request_dispatcher.h"

using namespace std;

TEST(LibraryTest, AlwaysOk) {

  TestBasic tb;
  tb.TestRocksDB();
  tb.TestMongoose();
  tb.TestJson();

  EXPECT_EQ(1,1);
}

TEST(DbHandlerTest, AlwaysOk) {

  TestBasic tb;
  tb.TestDbHandler();

  EXPECT_EQ(1,1);
}

TEST(DataHandlerTest, AlwaysOk) {

  TestBasic tb;
  tb.TestDataHandler();

  EXPECT_EQ(1,1);
}

TEST(YamlTest, ReadsFromFile) {

  // Create config file
  ofstream myfile;
  myfile.open ("config.yml");
  myfile << "# Configuración de conexión\nbindip: 127.0.0.1\nbindport: 8080\nlogfile: mylog.txt\nloglevel: debug";
  myfile.close();
  
  // Read config file
  ConfigParser yp;
  ConfigParser::Configuration config;
  config.bindip = "99";
  config.bindport = "99";
  config.logfile = "NOT.txt";
  config.loglevel = "NOT";
  yp.load_configuration(config);

  EXPECT_EQ(config.bindip,"127.0.0.1");
  EXPECT_EQ(config.bindport,"8080");
  EXPECT_EQ(config.logfile,"mylog.txt");
  EXPECT_EQ(config.loglevel,"debug");

}


TEST(FileHandlerTest, SaveAndLoadFile) {

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


TEST(RequestDispatcherTest, Checkpoint2Routine) {

//   Checkpoint #2  
//   + Post signup     IN: name/email/pass/token                  OUT: userId
//   + Get token       IN: email/pass                             OUT: userId/email/token
//   + Post file       IN: binStream/filename/userId/dirId/token  OUT: fileId
//   + Post dir        IN: dirname/userId/dirId/token             OUT: dirId
//   + Get dirInfo     IN: userId/dirId/token                     OUT: listaDeArchivos/listaDeSubcarpetas
//   + Get userInfo    IN: userId/token                           OUT: name/email
  
  // Init database. ¡Warning!: This test assumes an empty Database
  RequestDispatcher* rd=RequestDispatcher::getInstance();
  string db_path = "/tmp/testdb_checkpoint2";
  size_t max_quota = 9999;
  EXPECT_TRUE(rd->init(db_path,max_quota));
  
  
  // + Post signup     IN: name/email/pass/token             OUT: userId  
  // Parameters IN
  string email="mail@mail.com"; string password="1234"; string name="jake";
  string location="152.08;121.55"; string generated_token="1029384756"; string date="27/10/2015";
  // Parameters OUT
  string user_id="0"; int status=0;
  bool ok = rd->sign_up(email,password,name,location,generated_token,date,user_id,status);
  EXPECT_TRUE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  EXPECT_TRUE(user_id!="0");
  
  
  // + Get token       IN: email/pass                        OUT: userId/email/token
  // Parameters IN
  email="mail@mail.com"; password="1234"; generated_token="8475610293";
  // Parameters OUT
  string user_id_readed="0"; status=0;
  ok = rd->log_in(email,password,generated_token,user_id_readed,status);
  EXPECT_TRUE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  EXPECT_EQ(user_id,user_id_readed);

  
  // + Post file       IN: binStream/filename/userId/dirId/token    OUT: fileId
  // Parameters IN
  const char* p_bin_stream = "# Configuración de conexión\nip: 127.0.0.1\nport: 8080"; // Size: 54 bytes
  string save_size_stream = "54";
  string file_name = "archivo";
  string file_ext = "txt";
  user_id;
  string parent_dir_id = "0"; // 0 == ROOT DIR
  string token = generated_token;
  date="28/10/2015";
  // Parameters OUT
  string file_id = "0";
  status = 0;
  ok = rd->new_file(user_id,token,file_name,file_ext,date,p_bin_stream,save_size_stream,parent_dir_id,file_id,status);
  EXPECT_TRUE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  EXPECT_TRUE(file_id!="0");
  
  
  // + Post dir        IN: dirname/userId/parentDirId/token             OUT: dirId  
  // Parameters IN
  user_id;
  token;
  string dir_name = "miCarpeta";
  date="29/10/2015";
  parent_dir_id = "0"; // 0 == ROOT DIR
  // Parameters OUT
  string dir_id = "0";
  status = 0;
  ok = rd->new_directory(user_id,token,dir_name,date,parent_dir_id,dir_id,status);
  EXPECT_TRUE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  EXPECT_TRUE(dir_id!="0");

  // ** Another file post **
  // + Post file       IN: binStream/filename/userId/dirId/token    OUT: fileId
  // Parameters IN
  const char* p_bin_stream_2 = "# Configuración de conexión\nip: 123.4.5.6\nport: 9090"; // Size: 54 bytes
  string save_size_stream_2 = "54";
  string file_name_2 = "archivo_2";
  string file_ext_2 = "txt";
  user_id;
  string parent_dir_id_2 = "0"; // 0 == ROOT DIR
  string token_2 = generated_token;
  date="29/10/2015";
  // Parameters OUT
  string file_id_2 = "0";
  status = 0;
  ok = rd->new_file(user_id,token_2,file_name_2,file_ext_2,date,p_bin_stream_2,save_size_stream_2,parent_dir_id_2,file_id_2,status);
  EXPECT_TRUE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  EXPECT_TRUE(file_id_2!="0");
  
  
  // + Get dirInfo     IN: userId/dirId/token                     OUT: listaDeArchivos/listaDeSubcarpetas  
  // Parameters IN
  user_id;
  dir_id = "0"; // 0 == ROOT DIR
  token;
  // Parameters OUT
  DataHandler::dir_info_st dir_info;
  status = 0;
  ok = rd->get_directory_info(user_id,token,dir_id,dir_info,status);
  EXPECT_TRUE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }

  EXPECT_EQ(dir_info.date_last_mod,"29/10/2015");       // Last date saved
  EXPECT_EQ(dir_info.directories_contained,";2");       // IDs of directories contained, separated by semicolon
  EXPECT_EQ(dir_info.files_contained,";1;2");           // IDs of files contained, separated by semicolon
  EXPECT_EQ(dir_info.name,"root");                      // Dir name
  EXPECT_EQ(dir_info.owner,"1");                        // ID of owner
  EXPECT_EQ(dir_info.parent_directory,"no_parent");     // ID of parent dir. Special case: Root Dir.
  EXPECT_EQ(dir_info.tags,"");                          // Tags, separated by semicolon
  EXPECT_EQ(dir_info.size,"108");                       // Dir size (54*2==108)

  // ** For reding information of sub-directories and files contained in this directory **
  vector<RequestDispatcher::info_element_st> v_dir_elem_info;
  rd->get_directory_element_info_from_dir_info(user_id,token,dir_info,v_dir_elem_info,status);
  for(vector<RequestDispatcher::info_element_st>::iterator it = v_dir_elem_info.begin() ; it!=v_dir_elem_info.end() ; ++it) {
    RequestDispatcher::info_element_st ei = (*it);
    // *** For use in info_node.cc ***  Note: %lu=long unsigned 
    cout << "Element info in JSON Format: " ;
    printf("[{ \"id\": \"%lu\",  \"name\": \"%s\","
                                                                                                                "\"size\": \"%lu\" ,  \"type\": \"%s\",  \"cantItems\": \"%lu\", "
                                                                                                                "\"shared\": \"%s\",  \"lastModDate\": \"%s\"}]",ei.id,ei.name.c_str(),ei.size,ei.type.c_str(),ei.number_of_items,ei.shared.c_str(),ei.lastModDate.c_str());
    cout << endl;

  }
  
  // + Get userInfo    IN: userId/token                           OUT: name/email
  // Parameters IN  
  user_id;
  token;
  // Parameters OUT  
  DataHandler::user_info_st user_info;
  status = 0;
  ok = rd->get_user_info(user_id,token,user_info,status);
  EXPECT_TRUE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  
  EXPECT_EQ(user_info.dir_root,"1");                 // Internal ID of user root dir
  EXPECT_EQ(user_info.email,"mail@mail.com");        // User mail   
  EXPECT_EQ(user_info.location,"152.08;121.55");     // User location   
  EXPECT_EQ(user_info.name,"jake");                  // User name   
  EXPECT_EQ(user_info.shared_files,"");              // IDs of shared files, separated by semicolon
  EXPECT_EQ(user_info.user_quota_used,"108");        // Size of quota used (54*2==108)
    
}


int main(int argc, char **argv) {

  // Testing with GTest
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

