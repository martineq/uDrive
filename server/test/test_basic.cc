#include <iostream>
#include <cstdio>
#include <string>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "test_basic.h"


using namespace std;
using namespace rocksdb;

void TestBasic::TestRocksDB() {

  cout << "------------------------" << endl;
  cout << " ¡Hola Mundo! - RocksDB " << endl;
  cout << "------------------------" << endl; 

  DB* db; 
  Options options; 

  // Optimize RocksDB. This is the easiest way to get RocksDB to perform well
  options.IncreaseParallelism(); 
  options.OptimizeLevelStyleCompaction(); 

  // Create the DB if it's not already present
  options.create_if_missing = true; 

  // Open DB
  Status s = DB::Open(options, "/tmp/testdb", &db); 
  assert(s.ok()); 

  // Put key-value
  s = db->Put(WriteOptions(), "key1", "value"); 
  assert(s.ok()); 
  std::string value; 

  // Get value
  s = db->Get(ReadOptions(), "key1", &value); 
  assert(s.ok()); 
  assert(value == "value"); 

  // atomically apply a set of updates
  {
  WriteBatch batch; 
  batch.Delete("key1"); 
  batch.Put("key2", value); 
  s = db->Write(WriteOptions(), &batch); 
  }

  s = db->Get(ReadOptions(), "key1", &value); 
  assert(s.IsNotFound()); 

  db->Get(ReadOptions(), "key2", &value); 
  assert(value == "value"); 

  delete db; 

  cout << "Base de datos en: /tmp/testdb " << endl;

  cout << "---------------" << endl;
  cout << " Fin - RocksDB " << endl;
  cout << "---------------" << endl;
}


void TestBasic::TestJson() {

  cout << "------------------------" << endl;
  cout << " ¡Hola Mundo! - Jsoncpp " << endl;
  cout << "------------------------" << endl;

  // Crea desde el principio
  Json::Value fromScratch;
  Json::Value array;
  array.append("hello");
  array.append("world");
  fromScratch["hello"] = "world";
  fromScratch["number"] = 2;
  fromScratch["array"] = array;
  fromScratch["object"]["hello"] = "world";

  JsonPrint(fromScratch);

  // Escribe en manera legible
  Json::StyledWriter styledWriter;
  std::cout << styledWriter.write(fromScratch);

  // Parseo

  // Escribe en forma compacta
  Json::FastWriter fastWriter;
  std::string jsonMessage = fastWriter.write(fromScratch);

  Json::Value parsedFromString;
  Json::Reader reader;
  bool parsingSuccessful = reader.parse(jsonMessage, parsedFromString);
  if (parsingSuccessful){
    std::cout << styledWriter.write(parsedFromString) << std::endl;
  }

  cout << "---------------" << endl;
  cout << " Fin - Jsoncpp " << endl;
  cout << "---------------" << endl;
}

void TestBasic::JsonPrint(const Json::Value & value){
  std::cout << value["hello"];
  std::cout << value["number"];
  std::cout << value["array"][0] << value["array"][1];
  std::cout << value["object"]["hello"];
}

void TestBasic::TestMongoose() {

  cout << "-------------------------" << endl;
  cout << " ¡Hola Mundo! - Mongoose " << endl;
  cout << "-------------------------" << endl;

  struct mg_server *server;

  // Create and configure the server
  server = mg_create_server(NULL, ev_handler);
  mg_set_option(server, "listening_port", "8080");

  // Serve request. Hit Ctrl-C to terminate the program
  printf("Starting on port %s\n", mg_get_option(server, "listening_port"));
  for (int i=0;i<3;i++) {
    cout << "Mongoose - Poll Cicle No:"<< i+1 << endl;
    mg_poll_server(server, 1000);
  }

  // Cleanup, and free server instance
  mg_destroy_server(&server);

  cout << "----------------" << endl;
  cout << " Fin - Mongoose " << endl;
  cout << "----------------" << endl;

}

int TestBasic::ev_handler(struct mg_connection *conn, enum mg_event ev) {
  switch (ev) {
    case MG_AUTH: return MG_TRUE;
    case MG_REQUEST:
      mg_printf_data(conn, "Hello! Requested URI is [%s]", conn->uri);
      return MG_TRUE;
    default: return MG_FALSE;
  }
}

void TestBasic::TestDbHandler(){

  cout << "---------------------------" << endl;
  cout << " ¡Hola Mundo! - RocksDBDos " << endl;
  cout << "---------------------------" << endl;

  DbHandler dbh;

  assert(dbh.open("/tmp/testdb"));  
  assert(dbh.put("key1","value"));
  
  std::string value;
  bool found;
  assert(dbh.get("key1", &value, found));
  assert(found); 
  assert(value == "value"); 

  assert(dbh.get("key999", &value, found));
  assert(!found); 

  assert(dbh.put("key2","newvalue"));
  assert(dbh.get("key2", &value, found));
  assert(value == "newvalue"); 

  assert(dbh.put("key3","value3"));
  assert(dbh.put("key4","value4"));
  assert(dbh.put("key5","value5"));
  assert(dbh.put("key6","value6"));

  assert(dbh.erase("key2"));
  assert(dbh.get("key2", &value, found));
  assert(!found); 

  
  cout << "Base de datos en: /tmp/testdb " << endl;

  cout << "------------------" << endl;
  cout << " Fin - RocksDBDos " << endl;
  cout << "------------------" << endl;
  
}

void TestBasic::TestDataHandler(){
  cout << " Test DataHandler" << endl;
  // TODO (mart): add dir.size varieable to test
  DataHandler dh;
  
  // Turns on Database
  assert( dh.init("/tmp/testdb") );

  
  // Delete user "jake" if there was added previously to the database (just for testing purposes)
  delete_old_added_jake(dh);
  

  // Adds new user.
  string user_id_jake = "0";
  int status; // Used in case of error
  assert( dh.add_user("mail@mail.com","1234","jake","121.12;212.33","be16e465de64f0d2f2d83f3cfcd6370b","07-10-2015-15-20",user_id_jake,status) );
  assert( user_id_jake!="0" );


  
  // Gets user info
  DataHandler::user_info_st user_info_jack;
  assert( dh.get_user_info(user_id_jake,user_info_jack,status) );
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
  assert( !dh.get_user_password(mail_from_client,pass_saved_in_db,status) );
  assert( status == STATUS_KEY_NOT_FOUND );
  
  // Case B (wrong pass in client login)
  mail_from_client = "mail@mail.com";
  pass_from_client = "1234NOT";
  pass_saved_in_db = "0";
  assert( dh.get_user_password(mail_from_client,pass_saved_in_db,status) );
  assert( !(pass_saved_in_db==pass_from_client) );  
  
  // Case C (mail & pass OK in client login)
  // 1) Check email/pass obtained from client 
  mail_from_client = "mail@mail.com";
  pass_from_client = "1234";
  pass_saved_in_db = "0";
  assert( dh.get_user_password(mail_from_client,pass_saved_in_db,status) );
  assert( pass_saved_in_db==pass_from_client ); 
  //2) Save the generated token and give the user ID
  string new_token_generated = "be16e465de64f0d2f2d83f3cfcd60001";
  string user_id_obtained = "0";
  assert( dh.add_user_token("mail@mail.com",new_token_generated,user_id_obtained,status) );
  assert(user_id_obtained==user_id_jake);
  cout << " *Logs user with ID: "<< user_id_obtained << endl;


  // Sends root directory reference
  user_info_jack;
  assert( dh.get_user_info(user_id_jake,user_info_jack,status) );
  cout << " ** Sends to client Root Directory ID: "<< user_info_jack.dir_root << " **" << endl;
    
  
  // Sends directory info from a client request (CASE: "GET /info/users/{userId}/dir/{dirId}" request)
  string user_id_from_client = user_id_jake; // In this case, suppose correct user_id recieved from the client
  string dir_id_from_client = user_info_jack.dir_root; // In this case, suppose correct dir_id recieved from the client
  string token_from_client = "be16e465de64f0d2f2d83f3cfcd60001";

  // Token check (authentication)
  string token_from_db = "0";
  assert( dh.get_user_token(user_id_from_client,token_from_db,status) );
  assert( token_from_client==token_from_db );

  // In the real implementation, server must check if the dir_id belongs to the user_id (not implemented in this test)
  // and if the token recieved is valid
  // * dir_id - user_id check... *
  
  // Gets information of the directory requested, used in client response
  DataHandler::dir_info_st dir_info_jack;
  assert( dh.get_directory_info(dir_id_from_client,dir_info_jack,status) );
  cout << " ** Sends to client Directory info **" << endl;
  cout << "'Directory ID': " << user_id_from_client << endl;
  cout << "'Name': " << dir_info_jack.name << endl;
  cout << " 'Size' and 'cantItems' calculated from files contained: "  << dir_info_jack.files_contained << endl;
  cout << "...and calculated from sub directories contained: "  << dir_info_jack.directories_contained << endl;
  cout << "'type': d" << endl;


}


bool TestBasic::delete_old_added_jake(DataHandler& dh){
  int status;
  string pass_saved_in_db;
  
  if( dh.get_user_password("mail@mail.com",pass_saved_in_db,status) ){
    string token = "asdf";
    string user_id;
    dh.add_user_token("mail@mail.com",token,user_id,status);
    dh.delete_user(user_id,status);
    return true;
  }
  
  return false;
}






