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
  EXPECT_TRUE(dh.modify_user_info(user_id_jake,"newMail@mail.com","Jake","11.22;33.44",user_info_jack.shared_files,user_info_jack.user_quota_used,user_info_jack.files_deleted,status));
  
  // Modify dir info
  EXPECT_TRUE(dh.modify_directory_info(new_dir_id,"copy&paste","17-10-2015-15-20","favorite;most used","512",status));
  
  // Modify file info
  EXPECT_TRUE(dh.modify_file_info(new_file_id,"myBrabdNewFile","txt","17-10-2015-15-20","important","",user_id_jake,new_dir_id,status));
  
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
  string user_id = "999";
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


void generate_image(string &data, size_t &size); // Auxiliar function used by RequestDispatcherTest
TEST(RequestDispatcherTest, Checkpoint4Routine) {

  //Checkpoint #3  
  // + Post signup     IN: name/email/pass/token                  OUT: userId
  // + Get token       IN: email/pass                             OUT: userId/email/token
  // + Post file       IN: binStream/filename/userId/dirId/token  OUT: fileId
  // + Post dir        IN: dirname/userId/dirId/token             OUT: dirId
  // + Get dirInfo     IN: userId/dirId/token                     OUT: listaDeArchivos/listaDeSubcarpetas
  // + Get userInfo    IN: userId/token                           OUT: name/email

  // Init database. ¡Warning!: This test assumes an empty Database
  string db_path = "/tmp/testdb_checkpoint4";
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
  RequestDispatcher::dir_info_st dir_info;
  status = 0;
  EXPECT_TRUE(rd->check_token(user_id,token,status));
  ok = rd->get_directory_info(user_id,dir_id,dir_info,status);
  EXPECT_TRUE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }

  EXPECT_EQ(dir_info.date_last_mod,"29/10/2015");       // Last date saved
  EXPECT_EQ(dir_info.name,"root");                      // Dir name
  EXPECT_EQ(dir_info.owner,"1");                        // ID of owner
  EXPECT_EQ(dir_info.parent_directory,"no_parent");     // ID of parent dir. Special case: Root Dir.
  EXPECT_EQ(dir_info.tags,"");                          // Tags, separated by semicolon
  EXPECT_EQ(dir_info.size,"108");                       // Dir size (54*2==108)

  // ** For reding information of sub-directories and files contained in this directory **
  vector<RequestDispatcher::info_element_st> v_dir_elem_info = dir_info.directory_element_info;
  EXPECT_EQ(3,v_dir_elem_info.size());                        // Number of elements in this directory == 3
  
  // Get dir info from a non-root dir
  EXPECT_TRUE(rd->check_token(user_id,token,status));
  ok = rd->get_directory_info(user_id,sub_dir_id,dir_info,status);
  EXPECT_TRUE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  
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
  
  EXPECT_EQ("mail@mail.com",user_info.email);                // User mail   
  EXPECT_EQ("111.08",user_info.gps_lat);                     // User location   
  EXPECT_EQ("222.55",user_info.gps_lon);                     // User location   
  EXPECT_EQ("jake",user_info.first_name);                    // User name   
  EXPECT_EQ("the dog",user_info.last_name);                  // User name   
  EXPECT_EQ("108",user_info.user_quota_used);                 // Size of quota used (150-108==42)
  EXPECT_EQ("72.00%",user_info.user_quota_used_percentage);  // User quota percentage
  EXPECT_EQ("150",user_info.user_quota_total);               // User quota total
 
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
  RequestDispatcher::dir_info_st dir_info2;
  EXPECT_TRUE(rd->check_token(forbidden_user_id,"10244756",status));
  ok = rd->get_directory_info(forbidden_user_id,sub_dir_id,dir_info2,status);
  EXPECT_FALSE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  EXPECT_EQ(9,status); // STATUS_USER_FORBIDDEN==9
  
  // Use get_file_info() with forbidden user
  RequestDispatcher::file_info_st file_info2;
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
  
  // Use get_file_info() with correct user and token
  file_info2;
  EXPECT_TRUE(rd->check_token(user_id,token,status));
  ok = rd->get_file_info(user_id,file_id_2,file_info2,status);
  EXPECT_TRUE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }

  
  // Use get_file_stream() with authorized user
  char * p_file_stream_2 = nullptr;
  string size_2 = "0";
  EXPECT_TRUE(rd->check_token(user_id,token_2,status));
  ok = rd->get_file_stream(user_id,file_id_2,"1",p_file_stream_2,size_2,status);
  EXPECT_TRUE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  EXPECT_EQ("54",size_2);
  
  // Use get_file_stream() with forbidden user
  p_file_stream_2 = nullptr;
  size_2 = "0";
  EXPECT_TRUE(rd->check_token(user_id_second,"10244756",status));
  ok = rd->get_file_stream(user_id_second,file_id_2,"1",p_file_stream_2,size_2,status);
  EXPECT_FALSE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  EXPECT_EQ(9,status); // STATUS_USER_FORBIDDEN==9  
  
  // Share the file...
  EXPECT_TRUE(rd->check_token(user_id,token_2,status));
  string user_owner_id = user_id;
  string file_to_share_id = file_id_2;
  string user_shared_id = user_id_second;
  EXPECT_TRUE(rd->set_file_share(user_owner_id,file_to_share_id,user_shared_id,"06/11/15",status));
  // ...and get the file from te new user shared
  EXPECT_TRUE(rd->check_token(user_id_second,"10244756",status));
  ok = rd->get_file_stream(user_id_second,file_id_2,"1",p_file_stream_2,size_2,status);
  EXPECT_TRUE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  // ...List the shared files from an user
  vector< RequestDispatcher::info_element_st > shared_files;
  EXPECT_TRUE(rd->get_shared_files(user_shared_id,shared_files,status));
  EXPECT_EQ(1,shared_files.size()); 
  // ...Unshare the file from the same user...
  EXPECT_TRUE(rd->check_token(user_id,token_2,status));
  EXPECT_TRUE(rd->unset_file_share(user_owner_id,file_to_share_id,user_shared_id,"07/11/15",status));
  // ...and get the file from te new user un-shared (now is a forbidden user)
  EXPECT_TRUE(rd->check_token(user_id_second,"10244756",status));
  ok = rd->get_file_stream(user_id_second,file_id_2,"1",p_file_stream_2,size_2,status);
  EXPECT_FALSE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  EXPECT_EQ(9,status); // STATUS_USER_FORBIDDEN==9  
  // ... List the shared files from an user (now the user have 0 file shared)
  vector< RequestDispatcher::info_element_st > shared_files2;
  EXPECT_TRUE(rd->get_shared_files(user_shared_id,shared_files2,status));
  EXPECT_EQ(0,shared_files2.size()); 
  
  
  
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
  string size_stream = "0";
  status;
  EXPECT_TRUE(rd->check_token(user_id,token,status));
  EXPECT_TRUE(rd->get_dir_stream(user_id,root_dir_id,p_dir_stream,size_stream,status));
  EXPECT_EQ("1130",size_stream);  // Size of zip file (3 files): 1130 bytes

  // Create zip file with forbidden user
  sub_sub_dir_id;
  EXPECT_TRUE(rd->check_token(user_id,token,status));
  EXPECT_FALSE(rd->get_dir_stream(user_id_second,sub_sub_dir_id,p_dir_stream,size_stream,status));
  EXPECT_EQ(9,status); // STATUS_USER_FORBIDDEN==9  
  
  
  // Share the file...
  EXPECT_TRUE(rd->check_token(user_id,token_2,status));
  user_owner_id;
  file_to_share_id;
  user_shared_id;
  EXPECT_TRUE(rd->set_file_share(user_owner_id,file_to_share_id,user_shared_id,"06/11/15",status));
  // ...check if the user shared can acces the file
  shared_files.clear();
  EXPECT_TRUE(rd->get_shared_files(user_shared_id,shared_files,status));
  EXPECT_EQ(1,shared_files.size()); 
  // ...Delete the file from the user shared
  EXPECT_TRUE(rd->delete_file(user_shared_id,file_to_share_id,status));
  // ...Check file deleted  from user shared (file forbidden)
  EXPECT_TRUE(rd->check_token(user_id_second,"10244756",status));
  ok = rd->get_file_stream(user_id_second,file_to_share_id,"1",p_file_stream_2,size_2,status);
  EXPECT_FALSE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  EXPECT_EQ(9,status); // STATUS_USER_FORBIDDEN==9  
  // ...Check file non-deleted from user owner (the owner can still have the file)
  EXPECT_TRUE(rd->check_token(user_id,token_2,status));
  ok = rd->get_file_stream(user_id,file_to_share_id,"1",p_file_stream_2,size_2,status);
  EXPECT_TRUE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }  
  
  // ...Delete the file from the user owner
  EXPECT_TRUE(rd->delete_file(user_id,file_to_share_id,status));
  
  // ...Check file non-deleted from user owner  (the owner can NOT have the file)
  ok = rd->get_directory_info(user_id,dir_id,dir_info,status);
  EXPECT_TRUE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  EXPECT_EQ(2,dir_info.directory_element_info.size());   // Number of elements in this directory: 3-1==2
  
  // Recover deleted file
  EXPECT_TRUE(rd->recover_deleted_files(user_id,status));
  // Check again for the files recovered
  ok = rd->get_directory_info(user_id,dir_id,dir_info,status);
  EXPECT_TRUE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  EXPECT_EQ(3,dir_info.directory_element_info.size());   // Number of elements in this directory: 2+1=3
  
  // Modify file info
  EXPECT_TRUE(rd->modify_file_info(user_id,file_to_share_id,"file_renombrado","txt","08/11/15","favorito;público",status));
  
  // Delete file logically and physically
  EXPECT_TRUE(rd->delete_file(user_id,file_to_share_id,status));
  EXPECT_TRUE(rd->purge_deleted_files(user_id,status));
  // ...And get zip file without the file
  EXPECT_TRUE(rd->get_dir_stream(user_id,root_dir_id,p_dir_stream,size_stream,status));
  EXPECT_EQ("912",size_stream);  // Size of zip file (2 files): 912 bytes
  
  // Change directory information
  EXPECT_TRUE(rd->modify_directory_info(user_id,sub_dir_id,"dir_renombrado","09/11/15","importante",status));
  
  // Delete a directory with files and sub-directories
  EXPECT_TRUE(rd->delete_directory(user_id,sub_dir_id,status));
  // ...Check the directory obtained after directory delete
  ok = rd->get_directory_info(user_id,dir_id,dir_info,status);
  EXPECT_TRUE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  EXPECT_EQ(1,dir_info.directory_element_info.size());   // Number of elements in this directory: 2-1=1
  // ...And get zip file without the sub-dir
  EXPECT_TRUE(rd->get_dir_stream(user_id,root_dir_id,p_dir_stream,size_stream,status));
  EXPECT_EQ("372",size_stream);  // Size of zip file (1 file): 372 bytes
  // Save the zip file
  FileHandler fh;
  EXPECT_EQ(372,fh.save_file("carpeta_372.zip",p_dir_stream,stoul(size_stream,nullptr,10)));  // Size of zip file (1 file): 372 bytes

  
  // Change user info
  EXPECT_TRUE(rd->modify_user_info(user_id_second,"minuevo@mail.com.br","chihiro","IceK","122.34;45.33",status));
  
  // Delete user
  EXPECT_TRUE(rd->delete_user(user_id_second,status));
  // Check deleted user
  RequestDispatcher::user_info_st user_info_deleted;
  EXPECT_FALSE(rd->get_user_info(user_id_second,user_info_deleted,status));
  EXPECT_EQ(4,status); // STATUS_KEY_NOT_FOUND==4
  
  // Create the user image, and then save and load in the RequestDispatcher
  string data;
  size_t size_img;
  generate_image(data,size_img); // Create a binary file (jpg image)
  EXPECT_TRUE(rd->check_token(user_id,token,status));
  ok = rd->set_user_image(user_id,data.c_str(),to_string(size_img),status);
  EXPECT_TRUE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  char* p_image_stream;
  string size_img_loaded;
  ok = rd->get_user_image(user_id,p_image_stream,size_img_loaded,status);
  EXPECT_TRUE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  EXPECT_EQ(to_string(size_img),size_img_loaded);

  // Save an image with base64 format
  string data2("/9j/4AAQSkZJRgABAQAAAQABAAD/2wBDAAUDBAQEAwUEBAQFBQUGBwwIBwcHBw8LCwkMEQ8SEhEPERETFhwXExQaFRERGCEYGh0dHx8fExciJCIeJBweHx7/2wBDAQUFBQcGBw4ICA4eFBEUHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh7/wAARCAAyADIDASIAAhEBAxEB/8QAGgABAQADAQEAAAAAAAAAAAAAAAcFBggDBP/EAD4QAAEDAgIGBQgHCQAAAAAAAAEAAgMEEQUGBxIhMUFhEyIyUXEIFBUWI4GT0xczQkNSU1RVgpGSobHB0eH/xAAaAQACAwEBAAAAAAAAAAAAAAAABgMEBQcB/8QALBEAAQMCBAMHBQAAAAAAAAAAAQACBAMFERMhMQZB0RIUImFxgeEyQpHB8P/aAAwDAQACEQMRAD8A7LRFPtOObfV3K7qGll1cRxEOijsdscf238thsOZvwXjjgMSoJMhkak6q/YLPUGdMDxBkslFLLURxTPhc9jRbWabHju4jvBBX0es2H/l1H8o/2uZ9GuYDg2YhS1D7UVeQx9zsZJua7/B8R3K0FIt4vdxgSCwEdk6jTl8LQ4ffGusQVTo4aOGPPofhUelnjqadk8Rux4uF6LVsnYhqSGgld1X7Y78DxC2lNNpuDZ8VtYb7HyP9qvJVA0KhZ+EREWkq68quohpKSarqZGxQQxukke7c1oFyT4ALkjSHmafNeaqrFZC4Qk9HTRn7uIdkf3J5kqr+Ujm7zWhiypRS+2qQJawtO1sd+qz3kXPIDgVAw5Vqz8TgkXiW45lQRmHRu/r8LJ5fwarzFjlHg9E3WmqpAwEg2aOLjyAuTyC6PxTCHYP0FMJZJ4hE1rZZO0+wAJPPj71gvJzyj6Pwd+Z62K1VXN1KUOG1kN+1+8f6Ad6p+N0Ir6B8I+sHWjPP/u5ZF5tXfoZDfrGo6e/RM3B9J0Bma/79/Tl1WgxyOjkbJG4tc03BHAqgYRWsr6FlQ22tueO53FT14LHFrthBsQVlsq4iKOv6KR1oZrNNzuPApJ4auvcZeW8+B+h8jyP6Ke7jFzqXabuFu6Ii6slhRbylcpecUUObaKL2tOBDWho7TCeo8+BNie4jgFAXOOwDedgXcNbSwVtFPR1UTZaeeN0UrHbnNcLEHxBXPlVoFzI3EZ3UeJ4S6l6R3QGWWQP1L7NYCMgG2+xU0KNRqyW5zsGbnp7pSu1kdVlitTGIO48x1U4w6RzYRFrHqjZtX09I78Tv4qhRaEM2MN/SGC/Gl+Wvb6Fc1fr8G+NL8tdKp3iAGgZoUDrfJx0YVNCVvWhXK/rBmltXVRa1Bh5Estxse+/UZz2i55AjisgdCmav2hgvxpflqw6P8txZWyxT4WwtfNtkqZG7nym1z4CwA5AKhd79QbGLY78XO005DmVbgWyoawdVbgBr6rPoiLnyaUREQhEREIRERCEREQhf/9k=",1512);
  size_t size_img2 = 1512;
  EXPECT_TRUE(rd->check_token(user_id,token,status));
  ok = rd->set_user_image(user_id,data2.c_str(),to_string(size_img2),status);
  EXPECT_TRUE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  char* p_image_stream2;
  string size_img_loaded2;
  ok = rd->get_user_image(user_id,p_image_stream2,size_img_loaded2,status);
  EXPECT_TRUE(ok); if(!ok){ /* Check "status" */ std::cout <<"status ID: "<< status << std::endl; }
  EXPECT_EQ(to_string(size_img2),size_img_loaded2);
  size_t size_obtanined = stoul(size_img_loaded2,nullptr,10);
  string p_image_stream2_string(p_image_stream2,size_obtanined);
  EXPECT_STREQ(data2.c_str(),p_image_stream2_string.c_str());
  
  // Save an image with base64 format (Using HARDCODED_get_user_image())
  string p_image_stream3;
  EXPECT_TRUE(rd->HARDCODED_get_user_image(user_id,p_image_stream3,status));
  EXPECT_STREQ(data2.c_str(),p_image_stream3.c_str());
  
  // TODO(mart): implement this functions for test
  // Use get_file_info() from a user with a shared file (no the owner)
  // Use get_file_stream() from a user with a shared file (no the owner)
  
  
  // Delete instance
  delete rd;
    
  // Delete used temp folder
  system("rm -rf /tmp/testdb_checkpoint4");

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

void generate_image(string &data, size_t &size){
  
  const unsigned char gdrive_jpg[] = {
  0xff, 0xd8, 0xff, 0xe0, 0x00, 0x10, 0x4a, 0x46, 0x49, 0x46, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00,
  0xff, 0xdb, 0x00, 0x43, 0x00, 0x05, 0x03, 0x04, 0x04, 0x04, 0x03, 0x05, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x06, 0x07,
  0x0c, 0x08, 0x07, 0x07, 0x07, 0x07, 0x0f, 0x0b, 0x0b, 0x09, 0x0c, 0x11, 0x0f, 0x12, 0x12, 0x11, 0x0f, 0x11, 0x11, 0x13,
  0x16, 0x1c, 0x17, 0x13, 0x14, 0x1a, 0x15, 0x11, 0x11, 0x18, 0x21, 0x18, 0x1a, 0x1d, 0x1d, 0x1f, 0x1f, 0x1f, 0x13, 0x17,
  0x22, 0x24, 0x22, 0x1e, 0x24, 0x1c, 0x1e, 0x1f, 0x1e, 0xff, 0xdb, 0x00, 0x43, 0x01, 0x05, 0x05, 0x05, 0x07, 0x06, 0x07,
  0x0e, 0x08, 0x08, 0x0e, 0x1e, 0x14, 0x11, 0x14, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e,
  0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e,
  0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0xff, 0xc0,
  0x00, 0x11, 0x08, 0x00, 0x32, 0x00, 0x32, 0x03, 0x01, 0x22, 0x00, 0x02, 0x11, 0x01, 0x03, 0x11, 0x01, 0xff, 0xc4, 0x00,
  0x1a, 0x00, 0x01, 0x01, 0x00, 0x03, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07,
  0x05, 0x06, 0x08, 0x03, 0x04, 0xff, 0xc4, 0x00, 0x3e, 0x10, 0x00, 0x01, 0x03, 0x02, 0x02, 0x06, 0x05, 0x08, 0x07, 0x09,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x03, 0x04, 0x11, 0x05, 0x06, 0x07, 0x12, 0x21, 0x31, 0x41, 0x61,
  0x13, 0x22, 0x32, 0x51, 0x71, 0x08, 0x14, 0x15, 0x16, 0x23, 0x81, 0x93, 0xd3, 0x17, 0x33, 0x42, 0x43, 0x52, 0x53, 0x54,
  0x55, 0x82, 0x91, 0x92, 0xa1, 0xb1, 0xc1, 0xd1, 0xe1, 0xff, 0xc4, 0x00, 0x1a, 0x01, 0x00, 0x02, 0x03, 0x01, 0x01, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x03, 0x04, 0x05, 0x07, 0x01, 0xff, 0xc4, 0x00,
  0x2c, 0x11, 0x00, 0x01, 0x03, 0x02, 0x04, 0x03, 0x07, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
  0x02, 0x04, 0x03, 0x05, 0x11, 0x13, 0x21, 0x31, 0x06, 0x41, 0xd1, 0x12, 0x14, 0x22, 0x61, 0x71, 0x81, 0xe1, 0x32, 0x42,
  0x91, 0xc1, 0xf0, 0xff, 0xda, 0x00, 0x0c, 0x03, 0x01, 0x00, 0x02, 0x11, 0x03, 0x11, 0x00, 0x3f, 0x00, 0xec, 0xb4, 0x45,
  0x3e, 0xd3, 0x8e, 0x6d, 0xf5, 0x77, 0x2b, 0xba, 0x86, 0x96, 0x5d, 0x5c, 0x47, 0x11, 0x0e, 0x8a, 0x3b, 0x1d, 0xb1, 0xc7,
  0xf6, 0xdf, 0xcb, 0x61, 0xb0, 0xe6, 0x6f, 0xc1, 0x78, 0xe3, 0x80, 0xc4, 0xa8, 0x24, 0xc8, 0x64, 0x6a, 0x4e, 0xaa, 0xfd,
  0x82, 0xcf, 0x50, 0x67, 0x4c, 0x0f, 0x10, 0x64, 0xb2, 0x51, 0x4b, 0x2d, 0x44, 0x71, 0x4c, 0xf8, 0x5c, 0xf6, 0x34, 0x5b,
  0x59, 0xa6, 0xc7, 0x8e, 0xee, 0x23, 0xbc, 0x10, 0x57, 0xd1, 0xeb, 0x36, 0x1f, 0xf9, 0x75, 0x1f, 0xca, 0x3f, 0xda, 0xe6,
  0x7d, 0x1a, 0xe6, 0x03, 0x83, 0x66, 0x21, 0x4b, 0x50, 0xfb, 0x51, 0x57, 0x90, 0xc7, 0xdc, 0xec, 0x64, 0x9b, 0x9a, 0xef,
  0xf0, 0x7c, 0x47, 0x72, 0xb4, 0x14, 0x8b, 0x78, 0xbd, 0xdc, 0x60, 0x48, 0x2c, 0x04, 0x76, 0x4e, 0xa3, 0x4e, 0x5f, 0x0b,
  0x43, 0x87, 0xdf, 0x1a, 0xeb, 0x10, 0x55, 0x3a, 0x38, 0x68, 0xe1, 0x8f, 0x3e, 0x87, 0xe1, 0x51, 0xe9, 0x67, 0x8e, 0xa6,
  0x9d, 0x93, 0xc4, 0x6e, 0xc7, 0x8b, 0x85, 0xe8, 0xb5, 0x6c, 0x9d, 0x88, 0x6a, 0x48, 0x68, 0x25, 0x77, 0x55, 0xfb, 0x63,
  0xbf, 0x03, 0xc4, 0x2d, 0xa5, 0x34, 0xda, 0x6e, 0x0d, 0x9f, 0x15, 0xb5, 0x86, 0xfb, 0x1f, 0x23, 0xfd, 0xaa, 0xf2, 0x55,
  0x03, 0x42, 0xa1, 0x67, 0xe1, 0x11, 0x11, 0x69, 0x2a, 0xeb, 0xca, 0xae, 0xa2, 0x1a, 0x4a, 0x49, 0xaa, 0xea, 0x64, 0x6c,
  0x50, 0x43, 0x1b, 0xa4, 0x91, 0xee, 0xdc, 0xd6, 0x81, 0x72, 0x4f, 0x80, 0x0b, 0x92, 0x34, 0x87, 0x99, 0xa7, 0xcd, 0x79,
  0xaa, 0xab, 0x15, 0x90, 0xb8, 0x42, 0x4f, 0x47, 0x4d, 0x19, 0xfb, 0xb8, 0x87, 0x64, 0x7f, 0x72, 0x79, 0x92, 0xaa, 0xfe,
  0x52, 0x39, 0xbb, 0xcd, 0x68, 0x62, 0xca, 0x94, 0x52, 0xfb, 0x6a, 0x90, 0x25, 0xac, 0x2d, 0x3b, 0x5b, 0x1d, 0xfa, 0xac,
  0xf7, 0x91, 0x73, 0xc8, 0x0e, 0x05, 0x40, 0xc3, 0x95, 0x6a, 0xcf, 0xc4, 0xe0, 0x91, 0x78, 0x96, 0xe3, 0x99, 0x50, 0x46,
  0x61, 0xd1, 0xbb, 0xfa, 0xfc, 0x2c, 0x9e, 0x5f, 0xc1, 0xaa, 0xf3, 0x16, 0x39, 0x47, 0x83, 0xd1, 0x37, 0x5a, 0x6a, 0xa9,
  0x03, 0x01, 0x20, 0xd9, 0xa3, 0x8b, 0x8f, 0x20, 0x2e, 0x4f, 0x20, 0xba, 0x3f, 0x14, 0xc2, 0x1d, 0x83, 0xf4, 0x14, 0xc2,
  0x59, 0x27, 0x88, 0x44, 0xd6, 0xb6, 0x59, 0x3b, 0x4f, 0xb0, 0x00, 0x93, 0xcf, 0x8f, 0xbd, 0x60, 0xbc, 0x9c, 0xf2, 0x8f,
  0xa3, 0xf0, 0x77, 0xe6, 0x7a, 0xd8, 0xad, 0x55, 0x5c, 0xdd, 0x4a, 0x50, 0xe1, 0xb5, 0x90, 0xdf, 0xb5, 0xfb, 0xc7, 0xfa,
  0x01, 0xde, 0xa9, 0xf8, 0xdd, 0x08, 0xaf, 0xa0, 0x7c, 0x23, 0xeb, 0x07, 0x5a, 0x33, 0xcf, 0xfe, 0xee, 0x59, 0x17, 0x9b,
  0x57, 0x7e, 0x86, 0x43, 0x7e, 0xb1, 0xa8, 0xe9, 0xef, 0xd1, 0x33, 0x70, 0x7d, 0x27, 0x40, 0x66, 0x6b, 0xfe, 0xfd, 0xfd,
  0x39, 0x75, 0x5a, 0x0c, 0x72, 0x3a, 0x39, 0x1b, 0x24, 0x6e, 0x2d, 0x73, 0x4d, 0xc1, 0x1c, 0x0a, 0xa0, 0x61, 0x15, 0xac,
  0xaf, 0xa1, 0x65, 0x43, 0x6d, 0xad, 0xb9, 0xe3, 0xb9, 0xdc, 0x54, 0xf5, 0xe0, 0xb1, 0xc5, 0xae, 0xd8, 0x41, 0xb1, 0x05,
  0x65, 0xb2, 0xae, 0x22, 0x28, 0xeb, 0xfa, 0x29, 0x1d, 0x68, 0x66, 0xb3, 0x4d, 0xce, 0xe3, 0xc0, 0xa4, 0x9e, 0x1a, 0xba,
  0xf7, 0x19, 0x79, 0x6f, 0x3e, 0x07, 0xe8, 0x7c, 0x8f, 0x23, 0xfa, 0x29, 0xee, 0xe3, 0x17, 0x3a, 0x97, 0x69, 0xbb, 0x85,
  0xbb, 0xa2, 0x22, 0xea, 0xc9, 0x61, 0x45, 0xbc, 0xa5, 0x72, 0x97, 0x9c, 0x51, 0x43, 0x9b, 0x68, 0xa2, 0xf6, 0xb4, 0xe0,
  0x43, 0x5a, 0x1a, 0x3b, 0x4c, 0x27, 0xa8, 0xf3, 0xe0, 0x4d, 0x89, 0xee, 0x23, 0x80, 0x50, 0x17, 0x38, 0xec, 0x03, 0x79,
  0xd8, 0x17, 0x70, 0xd6, 0xd2, 0xc1, 0x5b, 0x45, 0x3d, 0x1d, 0x54, 0x4d, 0x96, 0x9e, 0x78, 0xdd, 0x14, 0xac, 0x76, 0xe7,
  0x35, 0xc2, 0xc4, 0x1f, 0x10, 0x57, 0x3e, 0x55, 0x68, 0x17, 0x32, 0x37, 0x11, 0x9d, 0xd4, 0x78, 0x9e, 0x12, 0xea, 0x5e,
  0x91, 0xdd, 0x01, 0x96, 0x59, 0x03, 0xf5, 0x2f, 0xb3, 0x58, 0x08, 0xc8, 0x06, 0xdb, 0xec, 0x54, 0xd0, 0xa3, 0x51, 0xab,
  0x25, 0xb9, 0xce, 0xc1, 0x9b, 0x9e, 0x9e, 0xe9, 0x4a, 0xed, 0x64, 0x75, 0x59, 0x62, 0xb5, 0x31, 0x88, 0x3b, 0x8f, 0x31,
  0xd5, 0x4e, 0x30, 0xe9, 0x1c, 0xd8, 0x44, 0x5a, 0xc7, 0xaa, 0x36, 0x6d, 0x5f, 0x4f, 0x48, 0xef, 0xc4, 0xef, 0xe2, 0xa8,
  0x51, 0x68, 0x43, 0x36, 0x30, 0xdf, 0xd2, 0x18, 0x2f, 0xc6, 0x97, 0xe5, 0xaf, 0x6f, 0xa1, 0x5c, 0xd5, 0xfa, 0xfc, 0x1b,
  0xe3, 0x4b, 0xf2, 0xd7, 0x4a, 0xa7, 0x78, 0x80, 0x1a, 0x06, 0x68, 0x50, 0x3a, 0xdf, 0x27, 0x1d, 0x18, 0x54, 0xd0, 0x95,
  0xbd, 0x68, 0x57, 0x2b, 0xfa, 0xc1, 0x9a, 0x5b, 0x57, 0x55, 0x16, 0xb5, 0x06, 0x1e, 0x44, 0xb2, 0xdc, 0x6c, 0x7b, 0xef,
  0xd4, 0x67, 0x3d, 0xa2, 0xe7, 0x90, 0x23, 0x8a, 0xc8, 0x1d, 0x0a, 0x66, 0xaf, 0xda, 0x18, 0x2f, 0xc6, 0x97, 0xe5, 0xab,
  0x0e, 0x8f, 0xf2, 0xdc, 0x59, 0x5b, 0x2c, 0x53, 0xe1, 0x6c, 0x2d, 0x7c, 0xdb, 0x64, 0xa9, 0x91, 0xbb, 0x9f, 0x29, 0xb5,
  0xcf, 0x80, 0xb0, 0x03, 0x90, 0x0a, 0x85, 0xde, 0xfd, 0x41, 0xb1, 0x8b, 0x63, 0xbf, 0x17, 0x3b, 0x4d, 0x39, 0x0e, 0x65,
  0x5b, 0x81, 0x6c, 0xa8, 0x6b, 0x07, 0x55, 0x6e, 0x00, 0x6b, 0xea, 0xb3, 0xe8, 0x88, 0xb9, 0xf2, 0x69, 0x44, 0x44, 0x42,
  0x11, 0x11, 0x10, 0x84, 0x44, 0x44, 0x21, 0x11, 0x11, 0x08, 0x5f, 0xff, 0xd9
  };
  
  unsigned int gdrive_jpg_len = 1133;
  
  std::string data_temp(
    reinterpret_cast< const char* >(gdrive_jpg),
    reinterpret_cast< const char* >(gdrive_jpg) + gdrive_jpg_len);
  
  data = data_temp;
  size = gdrive_jpg_len;
}
