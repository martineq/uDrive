#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <stdio.h>
#include <unistd.h>

// External libraries
#include "gtest/gtest.h"
#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"
#include "mongoose/mongoose.h"
#include "json/json.h"

#include "../src/db_handler.h"
#include "../src/data_handler.h"
#include "../src/config_parser.h"
#include "../src/file_handler.h"
#include "../src/request_dispatcher.h"

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
  
}


static int ev_handler(struct mg_connection *conn, enum mg_event ev); // Auxiliar function used by MongooseTest
TEST(MongooseTest, CreateServeClose) {

  struct mg_server *server;

  // Create and configure the server
  //server = mg_create_server(NULL, TestAux::ev_handler);
  server = mg_create_server(NULL, ev_handler);
  const char * errMsg = mg_set_option(server, "listening_port", "8080");
  EXPECT_STREQ(NULL, errMsg);
  
  // Serve request. Hit Ctrl-C to terminate the program
  printf("Starting on port %s\n", mg_get_option(server, "listening_port"));
  for (int i=0;i<3;i++) {
    cout << "Mongoose - Poll Cicle No:"<< i+1 << endl;
    mg_poll_server(server, 1000);
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

  EXPECT_TRUE(dbh.erase("key2"));
  EXPECT_TRUE(dbh.get("key2", &value, found));
  EXPECT_FALSE(found); 

}


TEST(DataHandlerTest, AlwaysOk) {

  // TODO (mart): add dir.size varieable to test
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
  cout << " 'Size' and 'cantItems' calculated from files contained: "  << dir_info_jack.files_contained << endl;
  cout << "...and calculated from sub directories contained: "  << dir_info_jack.directories_contained << endl;
  cout << "'type': d" << endl;


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

  EXPECT_NE(p_load_size_stream,0);

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
  string db_path = "/tmp/testdb_checkpoint2";
  size_t max_quota = 9999;
  RequestDispatcher* rd = nullptr;
  rd = RequestDispatcher::get_instance(db_path,max_quota);
  EXPECT_NE(rd,nullptr);
  
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
