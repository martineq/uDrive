#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// External libraries
#include "gtest/gtest.h"
#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"
#include "mongoose/mongoose.h"
#include "json/json.h"

// Source included in code coverage
#include "../src/db_handler.h"
#include "../src/data_handler.h"
#include "../src/config_parser.h"
#include "../src/file_handler.h"
#include "../src/request_dispatcher.h"
#include "../src/zip_handler.h"


using namespace std;


TEST(RocksDBTest, CreateWriteRead){

  rocksdb::DB* db; 
  rocksdb::Options options; 

  // Optimize RocksDB. This is the easiest way to get RocksDB to perform well
  options.IncreaseParallelism(); 
  options.OptimizeLevelStyleCompaction(); 

  // Create the DB if it's not already present
  options.create_if_missing = true; 

  // Open DB
  rocksdb::Status s = rocksdb::DB::Open(options, "/tmp/testRocksDb", &db); 
  EXPECT_TRUE(s.ok());

  // Put key-value
  s = db->Put(rocksdb::WriteOptions(), "key1", "value"); 
  EXPECT_TRUE(s.ok());
  
  // Get value
  std::string value; 
  s = db->Get(rocksdb::ReadOptions(), "key1", &value); 
  EXPECT_TRUE(s.ok());
  EXPECT_EQ(value,"value");

  // atomically apply a set of updates
  rocksdb::WriteBatch batch; 
  batch.Delete("key1"); 
  batch.Put("key2", value); 
  s = db->Write(rocksdb::WriteOptions(), &batch); 
  EXPECT_TRUE(s.ok());

  s = db->Get(rocksdb::ReadOptions(), "key1", &value); 
  EXPECT_TRUE(s.IsNotFound());

  db->Get(rocksdb::ReadOptions(), "key2", &value); 
  EXPECT_EQ(value,"value");

  delete db; 
  
  // Delete used temp folder
  system("rm -rf /tmp/testRocksDb");
  
}


static int ev_handler(struct mg_connection *conn, enum mg_event ev); // Auxiliar function used by MongooseTest
TEST(MongooseTest, CreateServeClose) {

  struct mg_server *server;

  // Create and configure the server
  server = mg_create_server(NULL, ev_handler);
  const char * errMsg = mg_set_option(server, "listening_port", "8080");
  EXPECT_STREQ(NULL,errMsg);
  
  // Serve request. Hit Ctrl-C to terminate the program
  printf("Starting on port %s\n", mg_get_option(server, "listening_port"));
  for (int i=0;i<3;i++) {
    cout << "Mongoose - Poll Cicle No:"<< i+1 << endl;
    mg_poll_server(server, 100);
  }

  // Cleanup, and free server instance
  mg_destroy_server(&server);

  EXPECT_EQ(1,1);
 
}


TEST(JsonTest, CreateWriteRead) {

  // Create from Scratch
  Json::Value fromScratch;
  Json::Value array;
  array.append("hello");
  array.append("world");
  fromScratch["hello"] = "world";
  fromScratch["number"] = 2;
  fromScratch["array"] = array;
  fromScratch["object"]["hello"] = "world";

  std::cout << fromScratch["hello"];
  std::cout << fromScratch["number"];
  std::cout << fromScratch["array"][0] << fromScratch["array"][1];
  std::cout << fromScratch["object"]["hello"];
    
  // Write readeable
  Json::StyledWriter styledWriter;
  std::cout << styledWriter.write(fromScratch);

  // Write compact
  Json::FastWriter fastWriter;
  std::string jsonMessage = fastWriter.write(fromScratch);

  Json::Value parsedFromString;
  Json::Reader reader;
  bool parsingSuccessful = reader.parse(jsonMessage, parsedFromString);
  EXPECT_EQ(true,parsingSuccessful);
  if (parsingSuccessful){
    std::cout << styledWriter.write(parsedFromString) << std::endl;
  }
  
}


TEST(DbHandlerTest, OpenWriteRead) {

  DbHandler dbh;

  EXPECT_TRUE(dbh.open("/tmp/testdb"));  
  EXPECT_TRUE(dbh.put("key1","value"));
  
  std::string value;
  bool found;
  EXPECT_TRUE(dbh.get("key1", &value, found));
  EXPECT_TRUE(found); 
  EXPECT_TRUE(value == "value"); 

  EXPECT_TRUE(dbh.get("key999", &value, found));
  EXPECT_FALSE(found); 

  EXPECT_TRUE(dbh.put("key2","newvalue"));
  EXPECT_TRUE(dbh.get("key2", &value, found));
  EXPECT_TRUE(value == "newvalue"); 

  EXPECT_TRUE(dbh.put("key3","value3"));
  EXPECT_TRUE(dbh.put("key4","value4"));
  EXPECT_TRUE(dbh.put("key5","value5"));
  EXPECT_TRUE(dbh.put("key6","value6"));
  
  dbh.clear_batch();
  dbh.erase_batch("key6");
  dbh.put_batch("key7","value7");
  EXPECT_TRUE(dbh.write_batch());

  EXPECT_TRUE(dbh.erase("key2"));
  EXPECT_TRUE(dbh.get("key2", &value, found));
  EXPECT_FALSE(found); 

}


TEST(DbHandlerTest, OpenError) {

  DbHandler dbh;
  
  // Do NOT open the database
  //EXPECT_TRUE(dbh.open("/tmp/testdb"));  
  
  std::string value;
  bool found;
  EXPECT_FALSE(dbh.get("key1", &value, found));
  EXPECT_FALSE(found); 
  
}


TEST(DataHandlerTest, UserAdd_GetInfoPassToken_GetDirInfo) {

  DataHandler dh;
  
  // Init database. ¡Warning!: This test assumes an empty Database
  EXPECT_TRUE( dh.init("/tmp/testdb") );

  // Adds new user.
  string user_id_jake = "0";
  int status; // Used in case of error
  EXPECT_TRUE( dh.add_user("mail@mail.com","1234","jake","121.12;212.33","be16e465de64f0d2f2d83f3cfcd6370b","07-10-2015-15-20",user_id_jake,status) );
  EXPECT_TRUE( user_id_jake!="0" );

  
  // Gets user info
  DataHandler::user_info_st user_info_jack;
  EXPECT_TRUE( dh.get_user_info(user_id_jake,user_info_jack,status) );
  cout << " New user info " << endl;
  cout << " ID: "<< user_id_jake << endl;
  cout << " Name: "<< user_info_jack.name << endl;
  cout << " Email: "<< user_info_jack.email << endl;
  cout << " Location: "<< user_info_jack.location << endl;
  cout << " Root Diretory ID: "<< user_info_jack.dir_root << endl;
  cout << " ID's of shared files: "<< user_info_jack.shared_files << endl;
  // *Send all necesary data to the client. *


  // User logs 3 times (3 different "GET /token" requests cases): Case A (wrong mail), Case B (wrong pass), Case C (mail & pass OK)
  // Case A (wrong mail in client login)
  // 1) Check email/pass obtained from client 
  string mail_from_client = "mailNOT@mail.com";
  string pass_from_client = "1234";
  string pass_saved_in_db = "0";
  EXPECT_FALSE( dh.get_user_password(mail_from_client,pass_saved_in_db,status) );
  EXPECT_TRUE( status == STATUS_KEY_NOT_FOUND );
  
  // Case B (wrong pass in client login)
  mail_from_client = "mail@mail.com";
  pass_from_client = "1234NOT";
  pass_saved_in_db = "0";
  EXPECT_TRUE( dh.get_user_password(mail_from_client,pass_saved_in_db,status) );
  EXPECT_FALSE( pass_saved_in_db==pass_from_client );  
  
  // Case C (mail & pass OK in client login)
  // 1) Check email/pass obtained from client 
  mail_from_client = "mail@mail.com";
  pass_from_client = "1234";
  pass_saved_in_db = "0";
  EXPECT_TRUE( dh.get_user_password(mail_from_client,pass_saved_in_db,status) );
  EXPECT_TRUE( pass_saved_in_db==pass_from_client ); 
  //2) Save the generated token and give the user ID
  string new_token_generated = "be16e465de64f0d2f2d83f3cfcd60001";
  string user_id_obtained = "0";
  EXPECT_TRUE( dh.add_user_token("mail@mail.com",new_token_generated,user_id_obtained,status) );
  EXPECT_TRUE(user_id_obtained==user_id_jake);
  cout << " *Logs user with ID: "<< user_id_obtained << endl;


  // Sends root directory reference
  user_info_jack;
  EXPECT_TRUE( dh.get_user_info(user_id_jake,user_info_jack,status) );
  cout << " ** Sends to client Root Directory ID: "<< user_info_jack.dir_root << " **" << endl;
    
  
  // Sends directory info from a client request (CASE: "GET /info/users/{userId}/dir/{dirId}" request)
  string user_id_from_client = user_id_jake; // In this case, suppose correct user_id recieved from the client
  string dir_id_from_client = user_info_jack.dir_root; // In this case, suppose correct dir_id recieved from the client
  string token_from_client = "be16e465de64f0d2f2d83f3cfcd60001";

  // Token check (authentication)
  string token_from_db = "0";
  EXPECT_TRUE( dh.get_user_token(user_id_from_client,token_from_db,status) );
  EXPECT_TRUE( token_from_client==token_from_db );

  // In the real implementation, server must check if the dir_id belongs to the user_id (not implemented in this test)
  // and if the token recieved is valid
  // * dir_id - user_id check... *
  
  // Gets information of the directory requested, used in client response
  DataHandler::dir_info_st dir_info_jack;
  EXPECT_TRUE( dh.get_directory_info(dir_id_from_client,dir_info_jack,status) );
  cout << " ** Sends to client Directory info **" << endl;
  cout << "'Directory ID': " << user_id_from_client << endl;
  cout << "'Name': " << dir_info_jack.name << endl;
  cout << "'Size': " << dir_info_jack.size << endl;
  cout << "'cantItems' calculated from files contained: "  << dir_info_jack.files_contained << endl;
  cout << "...and calculated from sub directories contained: "  << dir_info_jack.directories_contained << endl;

  cout << "'type': d" << endl;

  
  // Delete used temp folder
  system("rm -rf /tmp/testdb");
}


TEST(DataHandlerTest, UserDelete_AddDirFile_ModPassUserFileDir) {

  DataHandler dh;

  // Init database. ¡Warning!: This test assumes an empty Database
  EXPECT_TRUE( dh.init("/tmp/testdb") );

  // Adds new user.
  string user_id_jake = "0";
  int status; // Used in case of error
  EXPECT_TRUE( dh.add_user("UNmail@mail.com","1234","jake","121.12;212.33","be16e465de64f0d2f2d83f3cfcd6370b","07-10-2015-15-20",user_id_jake,status) );
  EXPECT_TRUE( user_id_jake!="0" );

  // Delete user
  EXPECT_TRUE( dh.delete_user(user_id_jake,status) );
  
  // Create previously deleted user
  user_id_jake = "0";
  EXPECT_TRUE( dh.add_user("OTROmail@mail.com","1234","jake","121.12;212.33","be16e465de64f0d2f2d83f3cfcd6370b","07-10-2015-15-20",user_id_jake,status) );
  EXPECT_TRUE( user_id_jake!="0" );

  // Add directory (previously, get user info)
  DataHandler::user_info_st user_info_jack;
  EXPECT_TRUE( dh.get_user_info(user_id_jake,user_info_jack,status) );
  string new_dir_id = "0";
  EXPECT_TRUE(dh.add_directory(user_id_jake,"copy_and_paste","07-10-2015-15-20",user_info_jack.dir_root,new_dir_id,status));
  EXPECT_NE("0",new_dir_id);

  // Add file
  string new_file_id = "0";
  EXPECT_TRUE(dh.add_file(user_id_jake,"myNewFile","txt","17-10-2015-15-20","512","1",new_dir_id,new_file_id,status));
  EXPECT_NE("0",new_file_id);

  // Modify user password
  EXPECT_TRUE(dh.modify_user_password(user_id_jake,"0303",status));
  
  // Modify user info (change mail)
  EXPECT_TRUE(dh.modify_user_info(user_id_jake,"newMail@mail.com","Jake","11.22;33.44",user_info_jack.shared_files,user_info_jack.user_quota_used,status));
  
  // Modify dir info
  EXPECT_TRUE(dh.modify_directory_info(new_dir_id,"copy&paste","17-10-2015-15-20","favorite;most used","512",status));
  
  // Modify file info
  EXPECT_TRUE(dh.modify_file_info(new_file_id,"myBrabdNewFile","txt","17-10-2015-15-20","important","",user_id_jake,status));

  // Delete file
  EXPECT_TRUE(dh.delete_file(new_file_id,status));
  
  // Delete dir
  EXPECT_TRUE(dh.delete_directory(new_dir_id,status));
  
  
  // Delete used temp folder
  system("rm -rf /tmp/testdb");
}


TEST(ConfigParserTest, ReadsFromFile) {

  // Create config file
  ofstream myfile;
  myfile.open ("config.yml");
  myfile << "# Configuración de conexión\nbindip: 127.0.0.1\nbindport: 8080\nlogfile: mylog.txt\nloglevel: debug\ndbpath: /tmp/testdb";
  myfile.close();
  
  // Read config file
  ConfigParser yp;
  ConfigParser::Configuration config;
  config.bindip = "99";
  config.bindport = "99";
  config.logfile = "NOT.txt";
  config.loglevel = "NOT";
  EXPECT_TRUE(yp.load_configuration(config));

  EXPECT_EQ(config.bindip,"127.0.0.1");
  EXPECT_EQ(config.bindport,"8080");
  EXPECT_EQ(config.logfile,"mylog.txt");
  EXPECT_EQ(config.loglevel,"debug");
  EXPECT_EQ(config.dbpath,"/tmp/testdb");

  // Delete used file
  remove("config.yml");
  
}


TEST(ConfigParserTest, ErrorHandle) {

  // Create config file
  ofstream myfile;
  myfile.open ("NOTconfig.yml");
  myfile << "# Configuración de conexión\nbindip: 127.0.0.1\nbindport: 8080\nlogfile: mylog.txt\nloglevel: debug\ndbpath: /tmp/testdb";
  myfile.close();
  
  // Read config file
  ConfigParser yp;
  ConfigParser::Configuration config;
  config.bindip = "99";
  config.bindport = "99";
  config.logfile = "NOT.txt";
  config.loglevel = "NOT";
  EXPECT_FALSE(yp.load_configuration(config));
  
  // Delete used file
  remove("NOTconfig.yml");
  
}


TEST(FileHandlerTest, SaveAndLoadFile) {

  FileHandler fh;
  
  // Save file
  string user_id = "111";
  string file_id = "222";
  string revision = "010";
  string file_name = user_id+file_id+revision;
  const char* p_save_file_stream = "# Configuración de conexión\nip: 127.0.0.1\nport: 8080"; // Size: 54 bytes
  size_t save_size_stream = 54;
  fh.save_file(file_name,p_save_file_stream,save_size_stream);
    
  // Load file  
  char* p_load_file_stream = NULL;
  size_t p_load_size_stream = fh.load_file(file_name,p_load_file_stream);

  EXPECT_NE(p_load_size_stream,0);

  std::cout <<"File size readed: "<< p_load_size_stream  << std::endl;
  std::cout <<"File content readed: " << std::endl;
  std::cout << p_load_file_stream<< std::endl;

  EXPECT_EQ(save_size_stream,p_load_size_stream);

}


TEST(RequestDispatcherTest, Checkpoint3Routine) {

  //Checkpoint #2  
  // + Post signup     IN: name/email/pass/token                  OUT: userId
  // + Get token       IN: email/pass                             OUT: userId/email/token
  // + Post file       IN: binStream/filename/userId/dirId/token  OUT: fileId
  // + Post dir        IN: dirname/userId/dirId/token             OUT: dirId
  // + Get dirInfo     IN: userId/dirId/token                     OUT: listaDeArchivos/listaDeSubcarpetas
  // + Get userInfo    IN: userId/token                           OUT: name/email

  // Init database. ¡Warning!: This test assumes an empty Database
  string db_path = "/tmp/testdb_checkpoint3";
  size_t max_quota = 150;
  RequestDispatcher* rd = nullptr;
  rd = RequestDispatcher::get_instance(db_path,max_quota);
  EXPECT_NE(rd,nullptr);
  
  // + Post signup     IN: name/email/pass/token             OUT: userId  
  // Parameters IN
  string email="mail@mail.com"; string password="1234"; string name="jake";
  string locationX="111.08"; string locationY="222.55"; string generated_token="1029384756"; string date="27/10/2015";
  // Parameters OUT
  string user_id="0"; int status=0;
  bool ok = rd->sign_up(email,password,name,"the dog",locationX,locationY,generated_token,date,user_id,status);
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
  EXPECT_TRUE(rd->check_token(user_id,token,status));
  ok = rd->new_file(user_id,file_name,file_ext,date,p_bin_stream,save_size_stream,parent_dir_id,file_id,status);
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
  EXPECT_TRUE(rd->check_token(user_id,token,status));
  ok = rd->new_directory(user_id,dir_name,date,parent_dir_id,dir_id,status);
  EXPECT_TRUE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  EXPECT_TRUE(dir_id!="0");
  string sub_dir_id = dir_id;
  
  
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
  EXPECT_TRUE(rd->check_token(user_id,token_2,status));
  ok = rd->new_file(user_id,file_name_2,file_ext_2,date,p_bin_stream_2,save_size_stream_2,parent_dir_id_2,file_id_2,status);
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
  EXPECT_TRUE(rd->check_token(user_id,token,status));
  ok = rd->get_directory_info(user_id,dir_id,dir_info,status);
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
  EXPECT_TRUE(rd->check_token(user_id,token,status));
  rd->get_directory_element_info_from_dir_info(user_id,dir_info,v_dir_elem_info,status);

  for(vector<RequestDispatcher::info_element_st>::iterator it = v_dir_elem_info.begin() ; it!=v_dir_elem_info.end() ; ++it) {
    RequestDispatcher::info_element_st ei = (*it);
    // *** For use in info_node.cc ***  Note: %lu=long unsigned 
    cout << "Element info in JSON Format: " ;
    printf("[{ \"id\": \"%lu\",  \"name\": \"%s\","
           "\"size\": \"%lu\" ,  \"type\": \"%s\",  \"cantItems\": \"%lu\", "
           "\"shared\": \"%s\",  \"lastModDate\": \"%s\"}]",
           ei.id,ei.name.c_str(),ei.size,ei.type.c_str(),ei.number_of_items,ei.shared.c_str(),ei.lastModDate.c_str());
    cout << endl;
  }
  
  // + Get userInfo    IN: userId/token                           OUT: name/email
  // Parameters IN  
  user_id;
  token;
  // Parameters OUT  
  RequestDispatcher::user_info_st user_info;
  status = 0;
  EXPECT_TRUE(rd->check_token(user_id,token,status));
  ok = rd->get_user_info(user_id,user_info,status);
  EXPECT_TRUE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  
  EXPECT_EQ("mail@mail.com",user_info.email);            // User mail   
  EXPECT_EQ("111.08",user_info.gps_lat);                 // User location   
  EXPECT_EQ("222.55",user_info.gps_lon);                 // User location   
  EXPECT_EQ("jake",user_info.first_name);                // User name   
  EXPECT_EQ("the dog",user_info.last_name);              // User name   
  EXPECT_EQ("108",user_info.user_quota_used);            // Size of quota used (54*2==108)
  EXPECT_EQ("72.00%",user_info.user_quota_percentage);   // User quota percentage
  EXPECT_EQ("150",user_info.user_quota_total);           // User quota total
 
  // *******************
  // *** Other tests *** 
  // *******************

  // Use various new_file() passing the max quota
  // Parameters IN
  p_bin_stream = "# Configuración de conexión\nip: 127.0.0.1\nport: 8080"; // Size: 54 bytes
  save_size_stream = "54";
  file_name = "archivoQueDesborda";
  file_ext = "txt";
  user_id;
  parent_dir_id = "0"; // 0 == ROOT DIR
  date="28/10/2015";
  // Parameters OUT
  string file2_id = "0";
  status = 0;
  EXPECT_TRUE(rd->check_token(user_id,token,status));
  ok = rd->new_file(user_id,file_name,file_ext,date,p_bin_stream,save_size_stream,parent_dir_id,file2_id,status);
  EXPECT_FALSE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  EXPECT_EQ(10,status); // STATUS_MAX_QUOTA_EXCEEDED==10
  EXPECT_TRUE(file2_id=="0");


  // Add second user and add new dir
  string user_id_second="0"; status=0;
  ok = rd->sign_up("mailsecond@mail.com","1234","finn","the human","152.08","121.55","10244756","02/11/2015",user_id_second,status);
  EXPECT_TRUE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  EXPECT_TRUE(user_id_second!="0");
  RequestDispatcher::user_info_st user_info_second;
  status = 0;
  EXPECT_TRUE(rd->check_token(user_id_second,"10244756",status));
  ok = rd->get_user_info(user_id_second,user_info_second,status);
  EXPECT_TRUE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }

  
  // Use get_directory_info() with forbidden user
  string forbidden_user_id = user_id_second;
  DataHandler::dir_info_st dir_info2;
  EXPECT_TRUE(rd->check_token(forbidden_user_id,"10244756",status));
  ok = rd->get_directory_info(forbidden_user_id,sub_dir_id,dir_info2,status);
  EXPECT_FALSE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  EXPECT_EQ(9,status); // STATUS_USER_FORBIDDEN==9
  
  // Use get_file_info() with forbidden user
  DataHandler::file_info_st file_info2;
  EXPECT_TRUE(rd->check_token(user_id_second,"10244756",status));
  ok = rd->get_file_info(user_id_second,file_id_2,file_info2,status);
  EXPECT_FALSE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  EXPECT_EQ(9,status); // STATUS_USER_FORBIDDEN==9
  
  // Use get_file_info() with wrong token value. (Verifies check_token() )
  file_info2;
  bool token_is_ok = rd->check_token(user_id_second,"00000000",status);
  EXPECT_FALSE(token_is_ok);
  EXPECT_EQ(6,status); // STATUS_WRONG_TOKEN==6
  if( token_is_ok ){
    // Never gets in
    //ok = rd->get_file_info(user_id_second,file_id_2,file_info2,status);
    //EXPECT_FALSE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  }
  
  
  // Use get_file_stream() with authorized user
  char * p_file_stream_2 = nullptr;
  size_t size_2 = 0;
  EXPECT_TRUE(rd->check_token(user_id,token_2,status));
  ok = rd->get_file_stream(user_id,file_id_2,"1",p_file_stream_2,size_2,status);
  EXPECT_TRUE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  EXPECT_EQ(54,size_2);
  
  // Use get_file_stream() with forbidden user
  p_file_stream_2 = nullptr;
  size_2 = 0;
  EXPECT_TRUE(rd->check_token(user_id_second,"10244756",status));
  ok = rd->get_file_stream(user_id_second,file_id_2,"1",p_file_stream_2,size_2,status);
  EXPECT_FALSE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  EXPECT_EQ(9,status); // STATUS_USER_FORBIDDEN==9  
  
  
  // Add sub-sub-directory
  string sub_sub_dir_id = "0";
  EXPECT_TRUE(rd->check_token(user_id,token,status));
  ok = rd->new_directory(user_id,"sub_sub",date,sub_dir_id,sub_sub_dir_id,status);
  EXPECT_TRUE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  EXPECT_TRUE(sub_sub_dir_id!="0");
  
   
  // Add file in sub-sub-directory
  const char* p_bin_stream_3 = "this is file 3"; // Size: 14 bytes
  string file_id_3 = "0";
  EXPECT_TRUE(rd->check_token(user_id,token,status));
  ok = rd->new_file(user_id,"file_3","txt",date,p_bin_stream_3,"14",sub_sub_dir_id,file_id_3,status);
  EXPECT_TRUE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  EXPECT_TRUE(file_id_3!="0");
  
  
  // Create zip file
  string root_dir_id = "0";
  char* p_dir_stream = nullptr;
  size_t size_stream = 0;
  status;
  EXPECT_TRUE(rd->check_token(user_id,token,status));
  EXPECT_TRUE(rd->get_dir_stream(user_id,root_dir_id,p_dir_stream,size_stream,status));
  EXPECT_EQ(1130,size_stream);  // Size of zip file: 1130 bytes
  
    
  // TODO(mart): implement this functions for test
  // Use get_directory_element_info_from_dir_info() with shared files
  // Use get_file_info() from a user with a shared file (no the owner)
  // Use get_file_stream() from a user with a shared file (no the owner)
  // Use decrease_user_quota_used()
  // Use decrease_dir_size_recursive()

  
  // Delete instance
  delete rd;
  
  
  // Delete used temp folder
  system("rm -rf /tmp/testdb_checkpoint3");

}



int main(int argc, char **argv) {

  // Testing with GTest
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


// ***************************
// *** Auxiliary Functions ***
// ***************************

static int ev_handler(struct mg_connection *conn, enum mg_event ev) {
  switch (ev) {
    case MG_AUTH: return MG_TRUE;
    case MG_REQUEST:
      mg_printf_data(conn, "Hello! Requested URI is [%s]", conn->uri);
      return MG_TRUE;
    default: return MG_FALSE;
  }
}
