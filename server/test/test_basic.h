// helloworld.h
#ifndef  HELLOWORLD_INC
#define  HELLOWORLD_INC

#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"
#include "mongoose/mongoose.h"
#include "json/json.h"
#include "../src/db_handler.h"
#include "../src/data_handler.h"


class TestBasic {

  public:
  void TestRocksDB();
  void TestJson();
  void TestMongoose();
  void TestDbHandler();
  void TestDataHandler();

  private:
  void JsonPrint(const Json::Value & value);
  static int ev_handler(struct mg_connection *conn, enum mg_event ev);
  bool delete_old_added_jake(DataHandler& dh);

}; 
 
#endif
