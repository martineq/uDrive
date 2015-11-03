#include "db_handler.h"


DbHandler::DbHandler(void){
  db_=nullptr;
}


DbHandler::~DbHandler(void){
  if(db_!=nullptr) delete db_;
}


bool DbHandler::open(std::string filename){
  rocksdb::Options options;
  
  // Optimize RocksDB. This is the easiest way to get RocksDB to perform well
  options.IncreaseParallelism();
  options.OptimizeLevelStyleCompaction();

  // Create the DB if it's not already present
  options.create_if_missing = true;

   

  // Open DB and return status
  return(check_status(rocksdb::DB::Open(options, filename, &db_)));  
}


bool DbHandler::put(std::string key, std::string value){
  if(db_==nullptr){ return false; }
  
  return(check_status(db_->Put(rocksdb::WriteOptions(),key,value)));
}


bool DbHandler::get(std::string key, std::string* value, bool& found){
  if(db_==nullptr){ 
    found = false;
    return false; 
  }

  rocksdb::Status s = db_->Get(rocksdb::ReadOptions(),key,value);
  found = !(s.IsNotFound());
  
  if(!found){
    std::cerr << "DB info: Key |"<< key << "| not found." << std::endl;
    return true; // "Key not found" is not an DB error
  }

  return(check_status(s));
}


bool DbHandler::erase(std::string key){
  return(check_status(db_->Delete(rocksdb::WriteOptions(),key)));
}


bool DbHandler::check_status(rocksdb::Status s){
  if( !(s.ok()) ){ std::cerr << "DB Status -> |"<< s.ToString() << "|" << std::endl; return false;
  }else{ return true; }
}


void DbHandler::erase_batch(std::string key){
  if(db_==nullptr){ return; }
  batch_.Delete(key);
}


void DbHandler::put_batch(std::string key, std::string value){
  if(db_==nullptr){ return; }
  batch_.Put(key,value);
}

void DbHandler::clear_batch(){
  if(db_==nullptr){ return; }
  batch_.Clear();
}


bool DbHandler::write_batch(){
  if(db_==nullptr){ return false; }
  return(check_status(db_->Write(rocksdb::WriteOptions(),&batch_)) );
}

