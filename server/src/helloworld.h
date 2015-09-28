// helloworld.h
#ifndef  HELLOWORLD_INC
#define  HELLOWORLD_INC

#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"
#include "mongoose/mongoose.h"
#include "json/json.h"

class HelloWorld {

  public:
  void EjemploRocksDB();
  void EjemploJson();
  void EjemploMongoose();

  private:
  void SalidaJson(const Json::Value & value);
  static int ev_handler(struct mg_connection *conn, enum mg_event ev);

}; 
 
#endif
