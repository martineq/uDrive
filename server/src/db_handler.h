#ifndef  DB_HANDLER_H
#define  DB_HANDLER_H

#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"
#include "rocksdb/write_batch.h"
#include "util/log.h"
#include <string>
#include <iostream>

class DbHandler {

  private:
    rocksdb::DB* db_;
    rocksdb::Status status_;
    rocksdb::WriteBatch batch_;
    
    bool check_status(rocksdb::Status s);

  public:
    DbHandler(void);
    ~DbHandler(void);
    bool open(std::string filename);
    bool put(std::string key, std::string value);
    bool get(std::string key, std::string* value, bool& found);
    bool erase(std::string key);

    void put_batch(std::string key, std::string value);
    void erase_batch(std::string key);
    void clear_batch();
    bool write_batch();
    
    
}; 

#endif // DB_HANDLER_H