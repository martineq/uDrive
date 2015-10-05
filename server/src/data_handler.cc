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


bool DataHandler::add_user(string email, string password, string name, string location, string token, string date, string& user_id, int& status){
  if(user_exists(email)){
    status = DH_STATUS_USER_ALREADY_EXISTS;
    return false;
  }

  // TODO (mart): create user_id
  user_id;
  
  // Creates the root directory
  string id_dir_root;
  bool status_dir;
  if(!add_directory(user_id,name,date,LABEL_EMPTY_STRING,id_dir_root,status)) return false;

  // Prepares data
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_EMAIL),email);
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_PASSWORD),password);
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_TOKEN),token);
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_NAME),name);
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_LOCATION),location);
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_DIR_ROOT),id_dir_root);
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_SHARED_FILE),LABEL_EMPTY_STRING);  

  // Saves data
  if(dbh_.write_batch()){
    // TODO (mart): add email to "index_user_id_from_user_email_"
    return true;
  }else{
    // If there is an error, delete the root directory
    delete_directory(id_dir_root,status);
    if(status==DH_STATUS_DATABASE_ERROR){ std::cerr << "Error deleting directory" << std::endl; }
    status = DH_STATUS_DATABASE_ERROR;
    return false;
  }
  
}


bool DataHandler::add_user_token(string email, string token, string& user_id, int& status){
  // TODO (mart): implement
  return true;
}


bool DataHandler::add_user_avatar(string user_id, string avatar_file_stream, int& status){
  // TODO (mart): implement
  return true;
}


bool DataHandler::add_directory(string user_id, string name, string date, string parent_dir_id, string& dir_id, int& status){
  // TODO (mart): implement
  return true;
}


bool DataHandler::add_file(string user_id, string name, string extension, string date, string directory, string& file_id, int& status){
  // TODO (mart): implement
  return true;
}


bool DataHandler::get_user_password(string email, string& password, int& status){
  // TODO (mart): implement
  return true;
}


bool DataHandler::get_user_token(string user_id, string& token, int& status){
  // TODO (mart): implement
  return true;
}


bool DataHandler::get_user_info(string user_id, user_info_st& user_info, int& status){
  // TODO (mart): implement
  return true;
}


bool DataHandler::get_user_avatar(string user_id, string& avatar_file_stream, int& status){
  // TODO (mart): implement
  return true;
}


bool DataHandler::get_directory_info(string user_id, dir_info_st& directory_info, int& status){
  // TODO (mart): implement
  return true;
}


bool DataHandler::get_file_info(string user_id, file_info_st& file_info, int& status){
  // TODO (mart): implement
  return true;
}


bool DataHandler::get_file_stream(string user_id, string& file_stream, int& status){
  // TODO (mart): implement
  return true;
}


bool DataHandler::delete_user(string user_id, int& status){
  // TODO (mart): implement
  return true;
}


bool DataHandler::delete_directory(string dir_id, int& status){
  // TODO (mart): implement
  return true;
}


bool DataHandler::delete_file(string file_id, int& status){
  // TODO (mart): implement
  return true;
}


bool DataHandler::modify_user_info(string user_id, string password, string name, string location, int& status){
  // TODO (mart): implement
  return true;
}


bool DataHandler::modify_directory_info(string dir_id, string name, string date, string tags, int& status){
  // TODO (mart): implement
  return true;
}


bool DataHandler::modify_file_info(string file_id, string name, string extension, string date, string tags, int& status){
  // TODO (mart): implement
  return true;
}


bool DataHandler::share_file(string file_id, string user_id_shared, int& status){
  // TODO (mart): implement
  return true;
}


bool DataHandler::unshare_file(string file_id, string user_id_unshared, int& status){
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
string DataHandler::generate_user_key(string user_id, string item_selected){
  return (PREFIX_USER+user_id+item_selected);
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
bool DataHandler::user_exists(string user_id){
  string value;
  bool found = false;
  return( dbh_.get(generate_user_key(user_id,SUFFIX_NAME),&value,found) && found==true );
}


bool DataHandler::create_id(string type_of_id, string& id){
  // TODO (mart): implement
  return true;
}


bool DataHandler::add_email_to_index(string email, string user_id){
  // TODO (mart): implement
    return true;
}


bool DataHandler::get_user_id_from_email(string email, string& user_id){
  // TODO (mart): implement
    return true;
}

