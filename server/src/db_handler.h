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
    
    /**
    * @brief Open the database with the specified "filename", creates the DB is it's not present.
    * Returns OK on success.
    * 
    * @param filename Name of the database file.
    * @return bool
    */
    bool open(std::string filename);
    
    /**
    * @brief Set the database entry for "key" to "value". If "key" already exists, it will be overwritten.
    * Returns OK on success, and a non-OK status on error.
    * 
    * @param key Name of the key stored in the database
    * @param value value to put on the register
    * @return bool
    */
    bool put(std::string key, std::string value);
    
    /**
    * @brief Gets the database entry for "key" to "value".
    *  If the database contains an entry for "key" store the corresponding value in *value and return OK.
    *  Returns false on error (an error NOT includes "key not found").
    *  If there is no entry for "key" leave *value unchanged and found indicates false.
    * 
    * @param key Name of the key stored in the database
    * @param value value to get on the register
    * @param found indicates if the value is founded
    * @return bool
    */
    bool get(std::string key, std::string* value, bool& found);
    
    /**
    * @brief   Remove the database entry (if any) for "key".  Returns OK on
    * success, and a non-OK status on error.  It is not an error if "key"
    * did not exist in the database.
    * 
    * @param key Name of the key stored in the database to be erased
    * @return bool
    */
    bool erase(std::string key);
    
    void put_batch(std::string key, std::string value);
    void erase_batch(std::string key);
    void clear_batch();
    bool write_batch();
    
    
}; 

#endif // DB_HANDLER_H