// db_handler.h
#ifndef  DB_HANDLER_INC
#define  DB_HANDLER_INC

#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"
#include <string>
#include <iostream>

class DbHandler {

  private:
    rocksdb::DB* db_;
    rocksdb::Status status_;
    
    bool check_status(rocksdb::Status s);

  public:
    DbHandler(void);
    ~DbHandler(void);
    bool open(std::string filename);
    bool put(std::string key, std::string value);
    bool get(std::string key, std::string* value, bool& found);
    bool erase(std::string key);

}; 
 
#endif
