#include <iostream>
#include <cstdio>
#include <string>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "helloworld.h"
#include "db_handler.h"

using namespace std;
using namespace rocksdb;

void HelloWorld::EjemploRocksDB() {

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


void HelloWorld::EjemploJson() {

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

  SalidaJson(fromScratch);

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

void HelloWorld::SalidaJson(const Json::Value & value){
  std::cout << value["hello"];
  std::cout << value["number"];
  std::cout << value["array"][0] << value["array"][1];
  std::cout << value["object"]["hello"];
}

void HelloWorld::EjemploMongoose() {

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

int HelloWorld::ev_handler(struct mg_connection *conn, enum mg_event ev) {
  switch (ev) {
    case MG_AUTH: return MG_TRUE;
    case MG_REQUEST:
      mg_printf_data(conn, "Hello! Requested URI is [%s]", conn->uri);
      return MG_TRUE;
    default: return MG_FALSE;
  }
}

void HelloWorld::EjemploRocksDBDos(){

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

  assert(!dbh.get("key999", &value, found));
  assert(!found); 

  assert(dbh.put("key2","newvalue"));
  assert(dbh.get("key2", &value, found));
  assert(value == "newvalue"); 

  assert(dbh.put("key3","value3"));
  assert(dbh.put("key4","value4"));
  assert(dbh.put("key5","value5"));
  assert(dbh.put("key6","value6"));

  assert(dbh.erase("key2"));
  assert(!dbh.get("key2", &value, found));
  assert(!found); 

  
  cout << "Base de datos en: /tmp/testdb " << endl;

  cout << "------------------" << endl;
  cout << " Fin - RocksDBDos " << endl;
  cout << "------------------" << endl;
  
}
