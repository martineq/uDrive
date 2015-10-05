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

  if(dbh_.open(DB_PATH)){
    // user_id ticket
    init_id_ticket(SUFFIX_USER_ID);
    
    // dir_id ticket
    init_id_ticket(SUFFIX_DIR_ID);
    
    // file_id ticket
    init_id_ticket(SUFFIX_FILE_ID);
  }else{
    db_error();
    return false;
  }

  return true;
}


/**
 * @brief Adds a new user on the DB. (Used in sing up)
 *        Creates a root directory and user ID. Returns true on success.
 *        On error returns false and a DataHandler status (see db_constants.h)
 * 
 * @param email 
 * @param password 
 * @param name 
 * @param location 
 * @param token 
 * @param date 
 * @param user_id returns user ID
 * @param status return DataHandler status ONLY if @return==false
 * @return bool
 */
bool DataHandler::add_user(string email, string password, string name, string location, string token, string date, string& user_id, int& status){

  // Verifies if the user exists
  string temp_user_id;
  if( get_email_user_id_index(email,temp_user_id,status) ){
    status = DH_STATUS_USER_ALREADY_EXISTS;
    return false;
  }else{
    if( status == DH_STATUS_DATABASE_ERROR ){ return false; }
  }
  
  // Creates the user_id
  if( !create_id(SUFFIX_USER_ID,user_id) ){
    status = DH_STATUS_DATABASE_ERROR;
    return false;
  }
  
  // Creates the root directory
  string id_dir_root;
  if(!add_directory(user_id,name,date,LABEL_EMPTY_STRING,id_dir_root,status)) return false;

  // Prepares data
  dbh_.clear_batch();
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_EMAIL),email);
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_PASSWORD),password);
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_TOKEN),token);
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_NAME),name);
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_LOCATION),location);
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_DIR_ROOT),id_dir_root);
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_SHARED_FILE),LABEL_EMPTY_STRING);  

  // Saves data
  if(dbh_.write_batch()){
    add_email_user_id_index(email,user_id);
    return true;
  }else{
    // If there is an error, delete the root directory
    delete_directory(id_dir_root,status);
    if(status==DH_STATUS_DATABASE_ERROR){ std::cerr << "Error deleting directory" << std::endl; }
    status = DH_STATUS_DATABASE_ERROR;
    return false;
  }
  
}


/**
 * @brief Adds a new token for a existent user (Used in logins AKA sing in). Returns the user_id on success.
 *        Returns true on success. On error returns false and a DataHandler status (see db_constants.h)
 * 
 * @param email ...
 * @param token ...
 * @param user_id returns user ID
 * @param status return DataHandler status ONLY if @return==false
 * @return bool
 */
bool DataHandler::add_user_token(string email, string token, string& user_id, int& status){

  // Verifies if the user exists
  if( !get_email_user_id_index(email,user_id,status) ){ return false; }

  // Adds new token
  return(dbh_put(generate_user_key(user_id,SUFFIX_TOKEN),token,status));
}


/**
 * @brief Adds a new directory for an user_id. ("new directory")
 *        Creates dir ID and links to their parent dir. Returns true on success.
 *        On error returns false and a DataHandler status (see db_constants.h)
 * 
 * @param user_id ...
 * @param name ...
 * @param date ...
 * @param parent_dir_id ...
 * @param dir_id returns directory ID
 * @param status return DataHandler status ONLY if @return==false
 * @return bool
 */
bool DataHandler::add_directory(string user_id, string name, string date, string parent_dir_id, string& dir_id, int& status){
  
  // Generates ID
  if( !create_id(SUFFIX_DIR_ID,dir_id) ){
    status = DH_STATUS_DATABASE_ERROR;
    return false;
  } 

  // Add dir_id to parent dir
  bool found = false;
  string directories_contained;
  if( !dbh_get(generate_dir_key(parent_dir_id,SUFFIX_DIRECTORIES_CONTAINED),&directories_contained,found,status) ){ return false; }
  directories_contained.append(";"+dir_id);
  if( !dbh_put(generate_dir_key(parent_dir_id,SUFFIX_DIRECTORIES_CONTAINED),directories_contained,status) ){ return false; }

  // Add info to new dir
  dbh_.clear_batch();
  dbh_.put_batch(generate_dir_key(dir_id,SUFFIX_NAME),name);
  dbh_.put_batch(generate_dir_key(dir_id,SUFFIX_DATE_LAST_MOD),date);
  dbh_.put_batch(generate_dir_key(dir_id,SUFFIX_USER_LAST_MOD),user_id);
  dbh_.put_batch(generate_dir_key(dir_id,SUFFIX_TAGS),LABEL_EMPTY_STRING);
  dbh_.put_batch(generate_dir_key(dir_id,SUFFIX_OWNER),user_id);
  dbh_.put_batch(generate_dir_key(dir_id,SUFFIX_PARENT_DIECTORY),parent_dir_id);
  dbh_.put_batch(generate_dir_key(dir_id,SUFFIX_FILES_CONTAINED),LABEL_EMPTY_STRING);
  dbh_.put_batch(generate_dir_key(dir_id,SUFFIX_DIRECTORIES_CONTAINED),LABEL_EMPTY_STRING);

  // Saves data to new dir
  if(!dbh_.write_batch()){
    status = DH_STATUS_DATABASE_ERROR;
    return false;
  }

  return true;
}


/**
 * @brief Adds a new file for an user_id and contained to a dir_id. ("new file")
 *        Creates file ID and links to their parent dir. Returns true on success.
 *        On error returns false and a DataHandler status (see db_constants.h)
 * 
 * @param user_id ...
 * @param name ...
 * @param extension ...
 * @param date ...
 * @param size ...
 * @param revision revision of the file
 * @param dir_id container directory of the file
 * @param file_id returns the file_id
 * @param status return DataHandler status ONLY if @return==false
 * @return bool
 */
bool DataHandler::add_file(string user_id, string name, string extension, string date, string size, string revision, string dir_id,
                           string& file_id, int& status){
  
  // Searches dir_id
  bool dir_found = false;
  string dir_files_contained;
  if( !dbh_get(generate_dir_key(dir_id,SUFFIX_FILES_CONTAINED),&dir_files_contained,dir_found,status) ) return false;
  
  // Generates ID for the new file
  if( !create_id(SUFFIX_FILE_ID,file_id) ){
    status = DH_STATUS_DATABASE_ERROR;
    return false;
  }
  
  // Add file_id to directory container
  dir_files_contained.append(";"+file_id);

  // Add info to new file and add file to directory
  dbh_.clear_batch();
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_NAME),name);
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_EXTENSION),extension);
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_DATE_LAST_MOD),date);
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_USER_LAST_MOD),user_id);
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_TAGS),LABEL_EMPTY_STRING);
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_OWNER),user_id);
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_SIZE),size);
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_DELETED_STATUS),DH_DELETED_STATUS_EXISTS);
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_USERS_SHARED),LABEL_EMPTY_STRING);
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_FILE_REVISION),revision);
  dbh_.put_batch(generate_dir_key(dir_id,SUFFIX_FILES_CONTAINED),dir_files_contained);

  // Saves data to new file
  if(!dbh_.write_batch()){
    status = DH_STATUS_DATABASE_ERROR;
    return false;
  }
  
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


bool DataHandler::get_directory_info(string user_id, dir_info_st& directory_info, int& status){
  // TODO (mart): implement
  return true;
}


bool DataHandler::get_file_info(string user_id, file_info_st& file_info, int& status){
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
 * @brief Returns the key for a query of an ticket type
 * 
 * @param ticket_type type of ticket selected
 * @return std::string
 */
string DataHandler::generate_ticket_key(string ticket_type){
  return (PREFIX_TICKET_LAST+ticket_type);
}


/**
 * @brief Obtains a new id for type_of_id. Returns true on success.
 * 
 * @param type_of_id the type of the requested id
 * @param id the new ID generated
 * @return bool
 */
bool DataHandler::create_id(string type_of_id, string& id){
  bool found = false;
  
  // Read and increment ticket id
  if( !(dbh_.get(generate_ticket_key(type_of_id),&id,found) && found==true) ){
    unsigned long new_id = ( (stoul(id,nullptr,10)) + 1 ); // Increment
    return( dbh_.put(generate_ticket_key(type_of_id),to_string(new_id)) );
  }else{
    return false;
  }

}


/**
 * @brief Adds an user_id for their corresponding email in a index. Returns true if the operation was successful
 * 
 * @param email ...
 * @param user_id ...
 * @return bool
 */
bool DataHandler::add_email_user_id_index(string email, string user_id){
  return( dbh_.put(PREFIX_INDEX_USER_ID_FROM_USER_EMAIL+email,user_id) );
}


/**
 * @brief Gets an user_id for their corresponding email from an index.
 *        Returns true if the operation was successful and the user_id was founded.
 *        Returns false and a status on error, or if the user_id is not found
 * 
 * @param email ...
 * @param user_id ...
 * @param status ...
 * @return bool
 */
bool DataHandler::get_email_user_id_index(string email, string& user_id, int& status){
  bool found = false;
  return( dbh_get(PREFIX_INDEX_USER_ID_FROM_USER_EMAIL+email,&user_id,found,status) );
}


/**
 * @brief Verifies the type_of_id ticket number (and creates if they not exist).
 * The ticket number is used in the assignment of ID's
 * 
 * @return void
 */
void DataHandler::init_id_ticket(string type_of_id){
  // Verifies the indexes
  string value;
  bool found = false;
  unsigned long id = 1;
  
  if( dbh_.get(generate_ticket_key(type_of_id),&value,found) ){
    if(!found){
      if( !(dbh_.put(generate_ticket_key(type_of_id),to_string(id))) ) { db_error();}
    }
  }else{
      db_error();
  }
  
}

/**
 * @brief Prints an error on std::cerr
 * 
 * @return void
 */
void DataHandler::db_error(){
  std::cerr << "DB error" << std::endl;
}


/**
 * @brief Wrapper for DbHandler::get(std::string key, std::string* value, bool& found). (used on member variable dbh_). 
 *        Includes the status for a get operation.
 * 
 * @param key ...
 * @param value ...
 * @param found ...
 * @param status ...
 * @return bool
 */
bool DataHandler::dbh_get(string key, string* value, bool& found, int& status){
  if( dbh_.get(key,value,found) ){
    if(!found){
      status = DH_STATUS_KEY_NOT_FOUND;
      return false;
    }
  }else{
    status = DH_STATUS_DATABASE_ERROR;
    return false;
  }

  return true;
}


/**
 * @brief Wrapper for DbHandler::put(std::string key, std::string value). (used on member variable dbh_). 
 *        Includes the status for a put operation.
 * 
 * @param key ...
 * @param value ...
 * @param status ...
 * @return bool
 */
bool DataHandler::dbh_put(string key, string value, int& status){
  if( !dbh_.put(key,value) ){
    status = DH_STATUS_DATABASE_ERROR;
    return false;
  }
  return true;
}
