#include <iostream>
#include <cstdio>
#include <string>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "LibraryTest.h"

using namespace std;
using namespace rocksdb;

/**
 *
 * Only for library testing purposes
 *
 */
void LibraryTest::RocksDBExample() {

	cout << "------------------------" << endl;
	cout << " ¡Hola Mundo! - RocksDB " << endl;
	cout << "------------------------" << endl;

	std::string kDBPath = "/tmp/testdb/"; //get_current_dir_name(); kDBPath+="tempBD";

	DB* db;
	Options options;

	// Optimiza RocksDB. This is the easiest way to get RocksDB to perform well
	options.IncreaseParallelism();
	options.OptimizeLevelStyleCompaction();

	// Crea la BD si no existiera antes
	options.create_if_missing = true;

	// Abre la BD
	Status s = DB::Open(options, kDBPath, &db);
	assert(s.ok());

	// Uso del Put()
	s = db->Put(WriteOptions(), "key1", "value");
	assert(s.ok());
	std::string value;

	// Uso del Get()
	s = db->Get(ReadOptions(), "key1", &value);
	assert(s.ok());
	assert(value == "value");

	// Aplico un grupo de actualizaciones atómicas
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

	cout << "Se creó y eliminó una base de datos en: " << kDBPath << endl;

	cout << "---------------" << endl;
	cout << " Fin - RocksDB " << endl;
	cout << "---------------" << endl;
}

void LibraryTest::MongooseExample() {

	struct mg_server *server;

	cout << "-------------------------" << endl;
	cout << " ¡Hola Mundo! - Mongoose " << endl;
	cout << "-------------------------" << endl;

	// Create and configure the server
	server = mg_create_server(NULL, ev_handler);
	mg_set_option(server, "listening_port", "8080");

	// Serve request. Hit Ctrl-C to terminate the program
	printf("Starting on port %s\n", mg_get_option(server, "listening_port"));
	for (int i=0;i<5;i++) {
	cout << "Mongoose - Poll Cicle No:"<< i+1 << endl;
	mg_poll_server(server, 1000);
	}

	// Cleanup, and free server instance
	mg_destroy_server(&server);

	cout << "----------------" << endl;
	cout << " Fin - Mongoose " << endl;
	cout << "----------------" << endl;

}

int LibraryTest::ev_handler(struct mg_connection *conn, enum mg_event ev) {
  switch (ev) {
    case MG_AUTH: return MG_TRUE;
    case MG_REQUEST:
      mg_printf_data(conn, "Hello! Requested URI is [%s]", conn->uri);
      return MG_TRUE;
    default: return MG_FALSE;
  }
}


void LibraryTest::JsonExample() {

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

void LibraryTest::JsonPrint(const Json::Value & value){
    std::cout << value["hello"];
    std::cout << value["number"];
    std::cout << value["array"][0] << value["array"][1];
    std::cout << value["object"]["hello"];
}

