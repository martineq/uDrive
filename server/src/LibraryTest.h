// LibraryTest.h
#ifndef  LIBRARYTEST_INC
#define  LIBRARYTEST_INC

// Library Headers
#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"
#include "mongoose/mongoose.h"
#include "json/json.h"

class LibraryTest {

   public:
   void RocksDBExample();
   void JsonExample();
   void MongooseExample();

   private:
   void JsonPrint(const Json::Value & value);
   static int ev_handler(struct mg_connection *, enum mg_event);


}; 
 
#endif
