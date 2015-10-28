#include "request_dispatcher.h"

RequestDispatcher::RequestDispatcher(){

}


RequestDispatcher::~RequestDispatcher(){

}


bool RequestDispatcher::init(string database_path, size_t max_user_quota){
  max_user_quota_ = max_user_quota;
  
  return dh_.init(database_path);
}


/**
 * @brief Adds a new user on the DB. (Used in sign up)
 *        Creates a "root" directory and user ID. Root directory is always id=0. Returns true on success.
 *        On error returns false and a DataHandler status (see db_constants.h)
 * 
 * @param email 
 * @param password 
 * @param name 
 * @param location 
 * @param token 
 * @param date 
 * @param user_id returns user ID
 * @param status returns DataHandler status ONLY if @return==false
 * @return bool
 */
bool RequestDispatcher::sign_up(string email, string password, string name, string location, string new_token, string date,
                                string& user_id, int& status){
  return dh_.add_user(email,password,name,location,new_token,date,user_id,status);
}


/**
 * @brief Verifies email/password and saves new_token. Returns true on success.
 *        On error returns false and a DataHandler status (see db_constants.h)
 * 
 * @param email ...
 * @param password ...
 * @param new_token ...
 * @param user_id returns user ID
 * @param status returns DataHandler status ONLY if @return==false
 * @return bool
 */
bool RequestDispatcher::log_in(string email, string password, string new_token, string& user_id, int& status){
  string saved_password;
  if( !dh_.get_user_password(email,saved_password,status) ){ return false; };

  if( password!=saved_password ){
    status = STATUS_WRONG_PASSWORD;
    return false;
  }

  return dh_.add_user_token(email,new_token,user_id,status);
}


bool RequestDispatcher::new_directory(string user_id, string user_token, string name, string date, string parent_dir_id,
                                      string& dir_id, int& status){
  if( !check_token(user_id,user_token,status) ){ return false; }

  if( parent_dir_id==LABEL_ZERO ){
    if( !get_root_dir_id(user_id,parent_dir_id,status) ){ return false; }
  }
  
  if( !dh_.add_directory(user_id,name,date,parent_dir_id,dir_id,status) ){ return false; }
  
  // Change parent_dir_id date
  if( !change_dir_date(parent_dir_id,date,status) ){ return false; }
  
  return true;
}


bool RequestDispatcher::new_file(string user_id, string user_token, string name, string extension, string date,
                                 const char* p_file_stream, string size, string parent_dir_id, string& file_id, int& status){
  if( !check_token(user_id,user_token,status) ){ return false; }

  // Check user quota
  string quota_used;
  if( !get_user_quota_used(user_id,quota_used,status) ){ return false; }
  if( (stoul(quota_used,nullptr,10) + stoul(size,nullptr,10)) > max_user_quota_ ){
    status = STATUS_MAX_QUOTA_EXCEEDED;
    return false;
  }

  if( parent_dir_id==LABEL_ZERO ){
    if( !get_root_dir_id(user_id,parent_dir_id,status) ){ return false; }
  }
    
  // Add logical file
  if( !dh_.add_file(user_id,name,extension,date,size,LABEL_REVISION_1,parent_dir_id,file_id,status) ){ return false; }
  
  // Add physical file
  if( fh_.save_file(user_id,file_id,LABEL_REVISION_1,p_file_stream,stoul(size,nullptr,10))==0 ){
    if( !dh_.delete_file(file_id,status) ){ return false; }
    status = STATUS_FAIL_SAVING_FILE;
    return false;
  }

  // Add size to user quota
  if( !increase_user_quota_used(user_id,size,status) ){ return false; }
  
  // Change parent_dir_id date
  if( !change_dir_date(parent_dir_id,date,status) ){ return false; }
  
  return true;
}


bool RequestDispatcher::get_user_info(string user_id, string user_token, DataHandler::user_info_st& user_info, int& status){
  if( !check_token(user_id,user_token,status) ){ return false; }

  return dh_.get_user_info(user_id,user_info,status);
}


bool RequestDispatcher::get_directory_info(string user_id, string user_token, string dir_id, DataHandler::dir_info_st& dir_info, int& status){
  if( !check_token(user_id,user_token,status) ){ return false; }
  
  if( dir_id==LABEL_ZERO ){
    if( !get_root_dir_id(user_id,dir_id,status) ){ return false; }
  }
  
  DataHandler::dir_info_st dir_info_temp;
  if( !dh_.get_directory_info(dir_id,dir_info_temp,status) ){ return false; }
  
  if( user_id!=dir_info_temp.owner ){
    status = STATUS_USER_FORBIDDEN;
    return false;
  }
  
  dir_info = dir_info_temp;
  return true;
}


bool RequestDispatcher::get_file_info(string user_id, string user_token, string file_id, DataHandler::file_info_st& file_info, int& status){
  if( !check_token(user_id,user_token,status) ){ return false; }

  DataHandler::file_info_st file_info_temp;
  if( !dh_.get_file_info(file_id,file_info_temp,status) ){ return false; }

  // Search for authorized user
  bool user_authorized = (file_info_temp.owner.compare(user_id)==0);
  if( !user_authorized ){
    vector<string> users_authorized = split_string(file_info_temp.users_shared,LABEL_STRING_DELIMITER);
    for(vector<string>::iterator it = users_authorized.begin() ; ( it!=users_authorized.end() && !user_authorized ) ; ++it) {
      if( (*it).compare(user_id)==0 ){ user_authorized = true; }
    }
  }
  
  if( !user_authorized ){
    status = STATUS_USER_FORBIDDEN;
    return false;
  }

  file_info = file_info_temp;
  return true;
}


bool RequestDispatcher::get_file_stream(string user_id, string user_token, string file_id, string revision,
                                        char*& p_file_stream, size_t& size_stream, int& status){
  if( !check_token(user_id,user_token,status) ){ return false; }
  
  DataHandler::file_info_st file_info_temp;
  if( !dh_.get_file_info(file_id,file_info_temp,status) ){ return false; }

  // Search for authorized user
  bool user_authorized = (file_info_temp.owner.compare(user_id)==0);
  if( !user_authorized ){
    vector<string> users_authorized = split_string(file_info_temp.users_shared,LABEL_STRING_DELIMITER);
    for(vector<string>::iterator it = users_authorized.begin() ; ( it!=users_authorized.end() && !user_authorized ) ; ++it) {
      if( (*it).compare(user_id)==0 ){ user_authorized = true; }
    }
  }
  
  if( !user_authorized ){
    status = STATUS_USER_FORBIDDEN;
    return false;
  }

  size_stream = fh_.load_file(user_id,file_id,LABEL_REVISION_1,p_file_stream);
  if( size_stream==0 ){
    status = STATUS_FAIL_LOADING_FILE;
    return false;
  }

  return true;
}


bool RequestDispatcher::delete_user(string user_id, string user_token, int& status){
  // TODO (mart): implement
//+ Borrar usuario -> get_user_token(), delete_user(). TODO: Verificar que el usr sea dueño. ¿Borrar todos sus archivos y carpetas?
  return false;
}


bool RequestDispatcher::delete_directory(string user_id, string user_token, string dir_id, int& status){
  // TODO (mart): implement
// + Borrar directorio -> get_user_token(), delete_directory(). TODO: Verificar que el usr sea dueño. Borrar todas las subcarpetas y archivos que contenga.
  return false;
}


bool RequestDispatcher::delete_file(string user_id, string user_token, string file_id, int& status){
  // TODO (mart): implement
//+ Borrar archivo -> get_user_token(), delete_file(). TODO: En caso de ser dueño, verificar si el archivo está compartido y quitar ese estado a los demás usuarios. En caso de ser compartido sólo quitarse de la lista de compartidos.
  return false;
}


bool RequestDispatcher::check_token(string user_id, string user_token, int& status){
  string saved_token;
  if( !dh_.get_user_token(user_id,saved_token,status) ){ return false; };
  
  if( user_token!=saved_token ){
    status = STATUS_WRONG_TOKEN;
    return false;
  }  

  return true;
}


vector<string> RequestDispatcher::split_string(string string_to_split, char delimiter){
    stringstream ss(string_to_split);
    string item;
    vector<string> tokens;
    while (getline(ss,item,delimiter)) {
      if( !item.empty() ){ tokens.push_back(item); }
    }
    return tokens;
}

bool RequestDispatcher::get_user_quota_used(string user_id, string& quota, int& status){
  DataHandler::user_info_st user_info;
  if( !dh_.get_user_info(user_id,user_info,status) ){ return false; }

  quota = user_info.user_quota_used;
  return true;
}


bool RequestDispatcher::increase_user_quota_used(string user_id, string quota_increased, int& status){
  DataHandler::user_info_st user_info;
  if( !dh_.get_user_info(user_id,user_info,status) ){ return false; }
  
  size_t new_quota = stoul(user_info.user_quota_used,nullptr,10) + stoul(quota_increased,nullptr,10);
  
  return( dh_.modify_user_info(user_id,user_info.email,user_info.name,user_info.location,
                               user_info.shared_files,to_string(new_quota),status) );
}


bool RequestDispatcher::decrease_user_quota_used(string user_id, string quota_decreased, int& status){
  DataHandler::user_info_st user_info;
  if( !dh_.get_user_info(user_id,user_info,status) ){ return false; }
  
  size_t new_quota = stoul(user_info.user_quota_used,nullptr,10) - stoul(quota_decreased,nullptr,10);
  
  return( dh_.modify_user_info(user_id,user_info.email,user_info.name,user_info.location,
                               user_info.shared_files,to_string(new_quota),status) );
}

bool RequestDispatcher::change_dir_date(string dir_id, string new_date, int& status){
  DataHandler::dir_info_st dir_info;
  if( !dh_.get_directory_info(dir_id,dir_info,status) ){ return false; }
  
  return( dh_.modify_directory_info(dir_id,dir_info.name,new_date,dir_info.tags,status) );
}

bool RequestDispatcher::get_root_dir_id(string user_id, string& root_dir_id, int& status){
  DataHandler::user_info_st user_info;
  if( !dh_.get_user_info(user_id,user_info,status) ){ return false; }
  root_dir_id = user_info.dir_root;
  return true;
}
