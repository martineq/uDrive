// LibraryTest.h
#ifndef  LIBRARYTEST_INC
#define  LIBRARYTEST_INC

// Library Headers
#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"
#include "../lib/mongoose/mongoose.h"
#include "json/json.h"

class LibraryTest {

   public:
   void EjemploRocksDB();
   void EjemploJson();
   void EjemploMongoose();

   private:
   void SalidaJson(const Json::Value & value);
   static int ev_handler(struct mg_connection *, enum mg_event);


}; 
 
#endif
