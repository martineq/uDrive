#include "data_handler.h"
#include "util/log.h"

DataHandler::DataHandler(void){
  
}


DataHandler::~DataHandler(void){

}


bool DataHandler::init(string database_path){
  Log(Log::LogMsgDebug) << "Opening bd... " <<database_path;
  if(dbh_.open(database_path)){

    // user_id ticket
    init_id_ticket(SUFFIX_USER_ID);

    // dir_id ticket
    init_id_ticket(SUFFIX_DIR_ID);

    // file_id ticket
    init_id_ticket(SUFFIX_FILE_ID);
    
    // list of users
    init_user_list();
    
  }else{ print_db_error(); return false; }

  return true;
}


bool DataHandler::add_user(string email, string password, string name, string location, string token, string date, string& user_id, int& status){

  // Verifies if the user exists
  string temp_user_id;
  if( get_email_user_id_index(email,temp_user_id,status) ){ status = STATUS_USER_ALREADY_EXISTS; return false;
  }else{
    if( status == STATUS_DATABASE_ERROR ){ return false; }
  }
  
  // Creates the user_id
  if( !create_id(SUFFIX_USER_ID,user_id) ){ status = STATUS_DATABASE_ERROR; return false; }
  
  // Creates the root directory
  string id_dir_root;
  if(!add_directory(user_id,LABEL_ROOT,date,LABEL_NO_PARENT_DIR,id_dir_root,status)){ return false; }

  // Prepares data
  dbh_.clear_batch();
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_EMAIL),email);
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_PASSWORD),password);
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_TOKEN),token);
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_NAME),name);
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_LOCATION),location);
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_DIR_ROOT),id_dir_root);
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_SHARED_FILES),LABEL_EMPTY_STRING);  
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_QUOTA_USED),LABEL_ZERO);  
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_FILES_DELETED),LABEL_EMPTY_STRING);  
  
  // Saves data
  if(dbh_.write_batch()){
    if( !add_email_user_id_index(email,user_id) ){ status = STATUS_DATABASE_ERROR; return false; }
    // Save user email in the list of users
    if( !add_email_to_list(email,status) ){ return false; }
  }else{// If there is an error, delete the root directory
    delete_directory(id_dir_root,status); if(status==STATUS_DATABASE_ERROR){ std::cerr << "Error deleting directory" << std::endl; }
    status = STATUS_DATABASE_ERROR; return false;
  }
  
  // Creates shared_files dir
  string shared_files_dir_id;
  if(!add_directory(user_id,LABEL_SHARED_FILES,date,id_dir_root,shared_files_dir_id,status)){ return false; }
  
  
  return true;
}


bool DataHandler::add_user_token(string email, string token, string& user_id, int& status){

  // Verifies if the user exists
  if( !get_email_user_id_index(email,user_id,status) ){ return false; }

  // Adds new token
  return(dbh_put(generate_user_key(user_id,SUFFIX_TOKEN),token,status));
}


bool DataHandler::add_directory(string user_id, string name, string date, string parent_dir_id, string& dir_id, int& status){
  
  // Generates ID
  if( !create_id(SUFFIX_DIR_ID,dir_id) ){ status = STATUS_DATABASE_ERROR; return false; } 

  // Add dir_id to parent dir
  string directories_contained;
  if( parent_dir_id != LABEL_NO_PARENT_DIR ){
    if( !dbh_get(generate_dir_key(parent_dir_id,SUFFIX_DIRECTORIES_CONTAINED),&directories_contained,status) ){ return false; }
    directories_contained.append(";"+dir_id);  
  }
  
  // Add info to new dir. 
  dbh_.clear_batch();
  dbh_.put_batch(generate_dir_key(dir_id,SUFFIX_NAME),name);
  dbh_.put_batch(generate_dir_key(dir_id,SUFFIX_DATE_LAST_MOD),date);
  dbh_.put_batch(generate_dir_key(dir_id,SUFFIX_TAGS),LABEL_EMPTY_STRING);
  dbh_.put_batch(generate_dir_key(dir_id,SUFFIX_OWNER),user_id);
  dbh_.put_batch(generate_dir_key(dir_id,SUFFIX_PARENT_DIRECTORY),parent_dir_id);
  dbh_.put_batch(generate_dir_key(dir_id,SUFFIX_FILES_CONTAINED),LABEL_EMPTY_STRING);
  dbh_.put_batch(generate_dir_key(dir_id,SUFFIX_DIRECTORIES_CONTAINED),LABEL_EMPTY_STRING);
  dbh_.put_batch(generate_dir_key(dir_id,SUFFIX_SIZE),LABEL_ZERO);
    
  // Writes new dir to parent dir.
  if( parent_dir_id != LABEL_NO_PARENT_DIR ){
    dbh_.put_batch(generate_dir_key(parent_dir_id,SUFFIX_DIRECTORIES_CONTAINED),directories_contained);
  }
  
  // Saves all data
  if(!dbh_.write_batch()){ status = STATUS_DATABASE_ERROR; return false; }

  return true;
}


bool DataHandler::add_file(string user_id, string name, string extension, string date, string size, string revision, string parent_dir_id,
                           string& file_id, int& status){
  
  // Search for parent directory
  string dir_files_contained;
  if( !dbh_get(generate_dir_key(parent_dir_id,SUFFIX_FILES_CONTAINED),&dir_files_contained,status) ) return false;
  
  // Generate ID for the new file
  if( !create_id(SUFFIX_FILE_ID,file_id) ){ status = STATUS_DATABASE_ERROR; return false; }
  
  // Add file_id to directory container
  dir_files_contained.append(LABEL_STRING_DELIMITER+file_id);

  // Add info to new file and add file to directory
  dbh_.clear_batch();
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_NAME),name);
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_EXTENSION),extension);
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_DATE_LAST_MOD),date);
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_USER_LAST_MOD),user_id);
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_TAGS),LABEL_EMPTY_STRING);
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_OWNER),user_id);
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_SIZE),size);
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_USERS_SHARED),LABEL_EMPTY_STRING);
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_FILE_REVISION),revision);
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_PARENT_DIRECTORY),parent_dir_id);
  dbh_.put_batch(generate_dir_key(parent_dir_id,SUFFIX_FILES_CONTAINED),dir_files_contained);
  
  // Saves data to new file
  if(!dbh_.write_batch()){ status = STATUS_DATABASE_ERROR; return false; }
  
  return true;
}


bool DataHandler::get_user_password(string email, string& password, int& status){
  
  // Verifies if the user exists and obtains user_id
  string user_id;
  if( !get_email_user_id_index(email,user_id,status) ){ return false; }

  // Get password
  return(dbh_get(generate_user_key(user_id,SUFFIX_PASSWORD),&password,status));
}


bool DataHandler::get_user_token(string user_id, string& token, int& status){
  return( dbh_get(generate_user_key(user_id,SUFFIX_TOKEN),&token,status) );
}


bool DataHandler::get_user_info(string user_id, user_info_st& user_info, int& status){

  if( !dbh_get(generate_user_key(user_id,SUFFIX_EMAIL),&(user_info.email),status) ) return false;
  if( !dbh_get(generate_user_key(user_id,SUFFIX_NAME),&(user_info.name),status) ) return false;
  if( !dbh_get(generate_user_key(user_id,SUFFIX_LOCATION),&(user_info.location),status) ) return false;
  if( !dbh_get(generate_user_key(user_id,SUFFIX_DIR_ROOT),&(user_info.dir_root),status) ) return false;
  if( !dbh_get(generate_user_key(user_id,SUFFIX_SHARED_FILES),&(user_info.shared_files),status) ) return false;
  if( !dbh_get(generate_user_key(user_id,SUFFIX_QUOTA_USED),&(user_info.user_quota_used),status) ) return false;
  if( !dbh_get(generate_user_key(user_id,SUFFIX_FILES_DELETED),&(user_info.files_deleted),status) ) return false;
  
  return true;
}


bool DataHandler::get_directory_info(string dir_id, dir_info_st& dir_info, int& status){

  if( !dbh_get(generate_dir_key(dir_id,SUFFIX_NAME),&(dir_info.name),status) ) return false;
  if( !dbh_get(generate_dir_key(dir_id,SUFFIX_DATE_LAST_MOD),&(dir_info.date_last_mod),status) ) return false;
  if( !dbh_get(generate_dir_key(dir_id,SUFFIX_TAGS),&(dir_info.tags),status) ) return false;
  if( !dbh_get(generate_dir_key(dir_id,SUFFIX_OWNER),&(dir_info.owner),status) ) return false;
  if( !dbh_get(generate_dir_key(dir_id,SUFFIX_PARENT_DIRECTORY),&(dir_info.parent_directory),status) ) return false;
  if( !dbh_get(generate_dir_key(dir_id,SUFFIX_FILES_CONTAINED),&(dir_info.files_contained),status) ) return false;
  if( !dbh_get(generate_dir_key(dir_id,SUFFIX_DIRECTORIES_CONTAINED),&(dir_info.directories_contained),status) ) return false;
  if( !dbh_get(generate_dir_key(dir_id,SUFFIX_SIZE),&(dir_info.size),status) ) return false;

  return true;
}


bool DataHandler::get_file_info(string file_id, file_info_st& file_info, int& status){
  
  if( !dbh_get(generate_file_key(file_id,SUFFIX_NAME),&(file_info.name),status) ) return false;  
  if( !dbh_get(generate_file_key(file_id,SUFFIX_EXTENSION),&(file_info.extension),status) ) return false; 
  if( !dbh_get(generate_file_key(file_id,SUFFIX_DATE_LAST_MOD),&(file_info.date_last_mod),status) ) return false; 
  if( !dbh_get(generate_file_key(file_id,SUFFIX_USER_LAST_MOD),&(file_info.user_last_mod),status) ) return false; 
  if( !dbh_get(generate_file_key(file_id,SUFFIX_TAGS),&(file_info.tags),status) ) return false; 
  if( !dbh_get(generate_file_key(file_id,SUFFIX_OWNER),&(file_info.owner),status) ) return false; 
  if( !dbh_get(generate_file_key(file_id,SUFFIX_SIZE),&(file_info.size),status) ) return false; 
  if( !dbh_get(generate_file_key(file_id,SUFFIX_USERS_SHARED),&(file_info.users_shared),status) ) return false; 
  if( !dbh_get(generate_file_key(file_id,SUFFIX_FILE_REVISION),&(file_info.revision),status) ) return false; 
  if( !dbh_get(generate_file_key(file_id,SUFFIX_PARENT_DIRECTORY),&(file_info.parent_directory),status) ) return false; 

  return true;
}


bool DataHandler::get_user_mail_list(string& email_list, int &status){
  return dbh_get(PREFIX_USER_EMAIL_LIST,&email_list,status);
}


bool DataHandler::get_email_user_id_index(string email, string& user_id, int& status){
  return( dbh_get(PREFIX_INDEX_USER_ID_FROM_USER_EMAIL+email,&user_id,status) );
}


bool DataHandler::delete_user(string user_id, int& status){

  string email;
  if( !dbh_get(generate_user_key(user_id,SUFFIX_EMAIL),&(email),status) ){ return false; }
  
  dbh_.clear_batch();
  dbh_.erase_batch(generate_user_key(user_id,SUFFIX_EMAIL));
  dbh_.erase_batch(generate_user_key(user_id,SUFFIX_PASSWORD));
  dbh_.erase_batch(generate_user_key(user_id,SUFFIX_TOKEN));
  dbh_.erase_batch(generate_user_key(user_id,SUFFIX_NAME));
  dbh_.erase_batch(generate_user_key(user_id,SUFFIX_LOCATION));
  dbh_.erase_batch(generate_user_key(user_id,SUFFIX_DIR_ROOT));
  dbh_.erase_batch(generate_user_key(user_id,SUFFIX_SHARED_FILES));
  dbh_.erase_batch(generate_user_key(user_id,SUFFIX_QUOTA_USED));
  dbh_.erase_batch(PREFIX_INDEX_USER_ID_FROM_USER_EMAIL+email);  
  
  // Saves data
  if(!dbh_.write_batch()){ status = STATUS_DATABASE_ERROR; return false; }
  
  if( !remove_email_from_list(email,status) ){ return false; }
  
  return true;
}


bool DataHandler::delete_directory(string dir_id, int& status){

  dbh_.clear_batch();
  dbh_.erase_batch(generate_dir_key(dir_id,SUFFIX_NAME));
  dbh_.erase_batch(generate_dir_key(dir_id,SUFFIX_DATE_LAST_MOD));
  dbh_.erase_batch(generate_dir_key(dir_id,SUFFIX_TAGS));
  dbh_.erase_batch(generate_dir_key(dir_id,SUFFIX_OWNER));
  dbh_.erase_batch(generate_dir_key(dir_id,SUFFIX_PARENT_DIRECTORY));
  dbh_.erase_batch(generate_dir_key(dir_id,SUFFIX_FILES_CONTAINED));
  dbh_.erase_batch(generate_dir_key(dir_id,SUFFIX_DIRECTORIES_CONTAINED));
  dbh_.erase_batch(generate_dir_key(dir_id,SUFFIX_SIZE));
  
  // Erases data
  if(!dbh_.write_batch()){ status = STATUS_DATABASE_ERROR; return false; }
  
  return true;
}


bool DataHandler::delete_file(string file_id, int& status){

  dbh_.clear_batch();
  dbh_.erase_batch(generate_file_key(file_id,SUFFIX_NAME));
  dbh_.erase_batch(generate_file_key(file_id,SUFFIX_EXTENSION));
  dbh_.erase_batch(generate_file_key(file_id,SUFFIX_DATE_LAST_MOD));
  dbh_.erase_batch(generate_file_key(file_id,SUFFIX_USER_LAST_MOD));
  dbh_.erase_batch(generate_file_key(file_id,SUFFIX_TAGS));
  dbh_.erase_batch(generate_file_key(file_id,SUFFIX_OWNER));
  dbh_.erase_batch(generate_file_key(file_id,SUFFIX_SIZE));
  dbh_.erase_batch(generate_file_key(file_id,SUFFIX_USERS_SHARED));
  dbh_.erase_batch(generate_file_key(file_id,SUFFIX_FILE_REVISION));
  dbh_.erase_batch(generate_file_key(file_id,SUFFIX_PARENT_DIRECTORY));
  dbh_.erase_batch(generate_file_key(file_id,SUFFIX_FILES_CONTAINED));

  // Erases data
  if(!dbh_.write_batch()){ status = STATUS_DATABASE_ERROR; return false; }
  
  return true;
}


bool DataHandler::modify_user_password(string user_id, string password, int& status){
  return( dbh_.put(generate_user_key(user_id,SUFFIX_PASSWORD),password) );
}


bool DataHandler::modify_user_info(string user_id, string email, string name, string location, string files_shared, string quota, string files_deleted, int& status){
  
  string old_email;
  if( !dbh_get(generate_user_key(user_id,SUFFIX_EMAIL),&(old_email),status) ) return false;
  
  dbh_.clear_batch();
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_EMAIL),email);
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_NAME),name);
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_LOCATION),location);
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_SHARED_FILES),files_shared);
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_QUOTA_USED),quota);
  dbh_.put_batch(generate_user_key(user_id,SUFFIX_FILES_DELETED),files_deleted);
  
  // Update index (if necessary)
  if( email.compare(old_email)!=0 ){
    dbh_.erase_batch(PREFIX_INDEX_USER_ID_FROM_USER_EMAIL+old_email);
    dbh_.put_batch(PREFIX_INDEX_USER_ID_FROM_USER_EMAIL+email,user_id);  // Refresh index email-user_id
    if(!remove_email_from_list(old_email,status)){ status = STATUS_DATABASE_ERROR; return false; }
    if(!add_email_to_list(email,status)){ status = STATUS_DATABASE_ERROR; return false; }
  }
  
  if(!dbh_.write_batch()){ status = STATUS_DATABASE_ERROR; return false; }

  return true;
}


bool DataHandler::modify_directory_info(string dir_id, string name, string date, string tags, string size, int& status){
  
  dbh_.clear_batch();
  dbh_.put_batch(generate_dir_key(dir_id,SUFFIX_NAME),name);
  dbh_.put_batch(generate_dir_key(dir_id,SUFFIX_DATE_LAST_MOD),date);
  dbh_.put_batch(generate_dir_key(dir_id,SUFFIX_TAGS),tags);
  dbh_.put_batch(generate_dir_key(dir_id,SUFFIX_SIZE),size);
  
  if(!dbh_.write_batch()){ status = STATUS_DATABASE_ERROR; return false; }

  return true;
}


bool DataHandler::modify_directory_files_contained(string dir_id, string files_contained, int& status){
  return( dbh_.put(generate_dir_key(dir_id,SUFFIX_FILES_CONTAINED),files_contained) );
}


bool DataHandler::modify_directory_dirs_contained(string dir_id, string directories_contained, int& status){
  return( dbh_.put(generate_dir_key(dir_id,SUFFIX_DIRECTORIES_CONTAINED),directories_contained) );
}


bool DataHandler::modify_file_info(string file_id, string name, string extension, string date, string tags, string users_shared, string user_id_modifier, string parent_dir, int& status){

  dbh_.clear_batch();
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_NAME),name);
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_EXTENSION),extension);
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_DATE_LAST_MOD),date);
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_USER_LAST_MOD),user_id_modifier);
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_TAGS),tags);
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_USERS_SHARED),users_shared);
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_PARENT_DIRECTORY),parent_dir);

  // Saves data to file
  if(!dbh_.write_batch()){ status = STATUS_DATABASE_ERROR; return false; }

  return true;
}


bool DataHandler::modify_file_revision(string file_id, string revision, int& status){

  dbh_.clear_batch();
  dbh_.put_batch(generate_file_key(file_id,SUFFIX_FILE_REVISION),revision);

  // Saves data to file
  if(!dbh_.write_batch()){ status = STATUS_DATABASE_ERROR; return false; }

  return true;
}


string DataHandler::generate_user_key(string user_id, string item_selected){
  return (PREFIX_USER+user_id+item_selected);
}


string DataHandler::generate_dir_key(string dir_id, string item_selected){
  return (PREFIX_DIR+dir_id+item_selected);
}


string DataHandler::generate_file_key(string file_id, string item_selected){
  return (PREFIX_FILE+file_id+item_selected);
}


string DataHandler::generate_ticket_key(string ticket_type){
  return (PREFIX_TICKET_LAST+ticket_type);
}


bool DataHandler::create_id(string type_of_id, string& id){
  bool found = false;
  
  // Read and increment ticket id
  if( (dbh_.get(generate_ticket_key(type_of_id),&id,found) && found==true) ){
    unsigned long new_id = ( (stoul(id,nullptr,10)) + 1 ); // Increment ticket
    return( dbh_.put(generate_ticket_key(type_of_id),to_string(new_id)) );
  }else{ return false; }

}


bool DataHandler::add_email_user_id_index(string email, string user_id){
  return( dbh_.put(PREFIX_INDEX_USER_ID_FROM_USER_EMAIL+email,user_id) );
}


void DataHandler::init_id_ticket(string type_of_id){
  // Verifies the indexes
  string value;
  bool found = false;
  unsigned long id = 1;
  
  if( dbh_.get(generate_ticket_key(type_of_id),&value,found) ){
    if(!found){
      if( !(dbh_.put(generate_ticket_key(type_of_id),to_string(id))) ) { print_db_error();}
    }
  }else{ print_db_error(); }
  
}


void DataHandler::init_user_list(){
  string value;
  bool found = false;

  if( dbh_.get(PREFIX_USER_EMAIL_LIST,&value,found) ){
    if(!found){
      if( !(dbh_.put(PREFIX_USER_EMAIL_LIST,LABEL_EMPTY_STRING)) ) { print_db_error();}
    }
  }else{ print_db_error(); }
  
}


void DataHandler::print_db_error(){ std::cerr << "DB error" << std::endl; }


bool DataHandler::dbh_get(string key, string* value, int& status){
  bool found = false;
  
  if( dbh_.get(key,value,found) ){
    if(!found){
      status = STATUS_KEY_NOT_FOUND;
      return false;
    }
  }else{ status = STATUS_DATABASE_ERROR; return false; }

  return true;
}


bool DataHandler::dbh_put(string key, string value, int& status){
  if( !dbh_.put(key,value) ){ status = STATUS_DATABASE_ERROR; return false; }
  return true;
}


bool DataHandler::add_email_to_list(string email, int status){
  string list_of_user_email;
  if( !dbh_get(PREFIX_USER_EMAIL_LIST,&list_of_user_email,status) ){ return false; }
  vector<string> list_splited = split_string(list_of_user_email,LABEL_STRING_DELIMITER);
  bool founded = false;
  for(vector<string>::iterator it = list_splited.begin() ; (it!=list_splited.end() && !founded ) ; ++it) {
    if( (*it).compare(email)==0 ){ founded = true; }
  }

  if(!founded){
    string new_key = LABEL_STRING_DELIMITER + email;
    list_of_user_email.append(new_key);
    if( !dbh_put(PREFIX_USER_EMAIL_LIST,list_of_user_email,status) ){ return false; }
  }
  
  return true;
}


bool DataHandler::remove_email_from_list(string email, int status){
  string list_of_user_email;
  if( !dbh_get(PREFIX_USER_EMAIL_LIST,&list_of_user_email,status) ){ return false; }

  // Remove the email by creating a new list without the email
  vector<string> list = split_string(list_of_user_email,LABEL_STRING_DELIMITER);  
  string new_list;
  for(vector<string>::iterator it = list.begin() ; it!=list.end() ; ++it) {
    if( (*it).compare(email)!=0 ){ new_list.append( LABEL_STRING_DELIMITER + (*it) ); }
  }
  
  if( !dbh_put(PREFIX_USER_EMAIL_LIST,new_list,status) ){ return false; }
  return true;
}


vector<string> DataHandler::split_string(string string_to_split, char delimiter){
    stringstream ss(string_to_split);
    string item;
    vector<string> tokens;
    while (getline(ss,item,delimiter)) {
      if( !item.empty() ){ tokens.push_back(item); }
    }
    return tokens;
}
