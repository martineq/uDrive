#include "data_handler.h"

DataHandler::DataHandler(void){

}


DataHandler::~DataHandler(void){

}


/**
 * @brief Initiates the database. 
 * Returns true if the DB is successfully open. On error returns false.
 * 
 * @param  ...
 * @return bool
 */
bool DataHandler::init(void){
  return(dbh_.open(DB_PATH));
}


bool DataHandler::add_user(string email, string password, string name, string location, string token, string date, int& status){
  if(user_exists(email)){
    status = DH_STATUS_USER_ALREADY_EXISTS;
    return false;
  }

  // Creates the root directory
  string id_dir_root;
  bool status_dir;
  if(!add_directory(email,token,name,date,LABEL_EMPTY_STRING,id_dir_root,status)) return false;

  // Prepares data
  dbh_.put_batch(generate_user_key(email,SUFFIX_PASSWORD),password);
  dbh_.put_batch(generate_user_key(email,SUFFIX_TOKEN),token);
  dbh_.put_batch(generate_user_key(email,SUFFIX_NAME),name);
  dbh_.put_batch(generate_user_key(email,SUFFIX_LOCATION),location);
  dbh_.put_batch(generate_user_key(email,SUFFIX_DIR_ROOT),id_dir_root);
  dbh_.put_batch(generate_user_key(email,SUFFIX_SHARED_FILE),LABEL_EMPTY_STRING);  

  // Saves data
  if(dbh_.write_batch()){
    return true;
  }else{
    // If there is an error, delete the root directory
    delete_directory(email,token,status);
    if(status==DH_STATUS_DATABASE_ERROR){ std::cerr << "Error deleting directory" << std::endl; }
    status = DH_STATUS_DATABASE_ERROR;
    return false;
  }
  
}


bool DataHandler::add_user_avatar(string email, string token, string file_stream, int& status){
  // TODO (mart): implement
  return true;  
}


bool DataHandler::add_directory(string email, string token, string name, string date, string parent_dir, string& dir_id, int& status){
  // TODO (mart): implement
  return true;  
}


bool DataHandler::add_file(string email, string token, string name, string extension, string date, string directory, string& file_id, int& status){
  // TODO (mart): implement
  return true;  
}


bool DataHandler::get_user_token(string email, string password, string& token, int& status){
  // TODO (mart): implement
  return true;  
}


bool DataHandler::get_user_info(string email, string token, string& user_info, int& status){
  // TODO (mart): implement
  return true;  
}


bool DataHandler::get_user_avatar(string email, string token, string& avatar_stream, int& status){
  // TODO (mart): implement
  return true;  
}


bool DataHandler::get_directory(string email, string token, string& directory_info, int& status){
  // TODO (mart): implement
  return true;  
}


bool DataHandler::get_shared_directory(string email, string token, string& shared_directory_info, int& status){
  // TODO (mart): implement
  return true;  
}


bool DataHandler::get_file(string email, string token, string& file_stream, int& status){
  // TODO (mart): implement
  return true;  
}


bool DataHandler::delete_user(string email, string token, int& status){
  // TODO (mart): implement
  return true;  
}


bool DataHandler::delete_directory(string email, string token, int& status){
  // TODO (mart): implement
  return true;  
}


bool DataHandler::delete_file(string email, string token, int& status){
  // TODO (mart): implement
  return true;  
}


bool DataHandler::modify_user_info(string email, string token, string password, string name, string location, int& status){
  // TODO (mart): implement
  return true;  
}


bool DataHandler::modify_directory(string email, string token, string name, string date, string tags, int& status){
  // TODO (mart): implement
  return true;  
}


bool DataHandler::modify_file_info(string email, string token, string name, string extension, string date, string user, int& status){
  // TODO (mart): implement
  return true;  
}


bool DataHandler::share_file(string email_owner, string token, string email_shared, int& status){
  // TODO (mart): implement
  return true;  
}


bool DataHandler::unshare_file(string email_owner, string token, string email_shared, int& status){
  // TODO (mart): implement
  return true;  
}


/**
 * @brief Returns the key for a query of an user item
 * 
 * @param email email of the user
 * @param item_selected item selected of the user for query
 * @return std::string
 */
string DataHandler::generate_user_key(string email, string item_selected){
  return (PREFIX_USER+email+item_selected);
}


/**
 * @brief Returns the key for a query of an directory item
 * 
 * @param dir_id ID of the selected directory
 * @param item_selected item selected of the user for query
 * @return std::string
 */
string DataHandler::generate_dir_key(string dir_id, string item_selected){
  return (PREFIX_DIR+dir_id+item_selected);
}

/**
 * @brief Returns the key for a query of an file item
 * 
 * @param file_id ID of the selected file
 * @param item_selected item selected of the user for query
 * @return std::string
 */
string DataHandler::generate_file_key(string file_id, string item_selected){
  return (PREFIX_FILE+file_id+item_selected);
}

/**
 * @brief Verifies if the user already exists. Returns true if exists, if not (or there is an error) returns false
 * 
 * @param email email of the user
 * @return bool
 */
bool DataHandler::user_exists(string email){
  string value;
  bool found = false;
  return( dbh_.get(generate_user_key(email,SUFFIX_NAME),&value,found) && found==true );
}

