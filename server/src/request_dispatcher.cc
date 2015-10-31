#include "request_dispatcher.h"

RequestDispatcher* RequestDispatcher::myrd_ = nullptr;


RequestDispatcher::RequestDispatcher(string database_path,size_t max_user_quota){
  
  if( !init_ok_){
    init_ok_ = init(database_path,max_user_quota);
    if (!init_ok_){ Log(Log::LogMsgError) << "DB init fail"; }
  }

}
 
 
RequestDispatcher::~RequestDispatcher(){

}


bool RequestDispatcher::init(string database_path, size_t max_user_quota){
  max_user_quota_ = max_user_quota;
  return dh_.init(database_path);
}


bool RequestDispatcher::sign_up(string email, string password, string name, string location, string new_token, string date,
                                string& user_id, int& status){
  return dh_.add_user(email,password,name,location,new_token,date,user_id,status);
}


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
  if( !change_dir_date_recursive(parent_dir_id,date,status) ){ return false; }
  
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
  
  // Change parent_dir_id date & add size
  if( !change_dir_date_recursive(parent_dir_id,date,status) ){ return false; }

  // Recursive size add
  if( !increase_dir_size_recursive(parent_dir_id,size,status) ){ return false; }
  
  return true;
}


bool RequestDispatcher::get_user_info(string user_id, string user_token, DataHandler::user_info_st& user_info, int& status){
  if( !check_token(user_id,user_token,status) ){ return false; }

  return dh_.get_user_info(user_id,user_info,status);
}


bool RequestDispatcher::get_directory_info(string user_id, string user_token, string dir_id, DataHandler::dir_info_st& dir_info, int& status){
  if( !check_token(user_id,user_token,status) ){ return false; }
  
  bool is_root_dir = (dir_id==LABEL_ZERO);
  
  if( is_root_dir ){
    if( !get_root_dir_id(user_id,dir_id,status) ){ return false; }
  }
  
  DataHandler::dir_info_st dir_info_temp;
  if( !dh_.get_directory_info(dir_id,dir_info_temp,status) ){ return false; }
  
  if( user_id!=dir_info_temp.owner ){
    status = STATUS_USER_FORBIDDEN;
    return false;
  }

  // Assign dir_info value
  dir_info = dir_info_temp;
  
  if( !is_root_dir ){
    // Check if the parent dir is the root dir, and then, format the root dir ID (change to id==0)
    DataHandler::dir_info_st parent_dir_info_temp;
    if( !dh_.get_directory_info(dir_info.parent_directory,parent_dir_info_temp,status) ){ return false; }
    if( parent_dir_info_temp.parent_directory == LABEL_NO_PARENT_DIR ){
      dir_info.parent_directory = LABEL_ZERO;
    }   
  }
  
  return true;
}


bool RequestDispatcher::get_directory_element_info_from_dir_info(string user_id, string user_token, DataHandler::dir_info_st dir_info,
                                                                 vector< RequestDispatcher::info_element_st >& directory_element_info, int& status){
  directory_element_info.clear();
  
  // Fill subdir info
  vector<string> subdir_ids = split_string(dir_info.directories_contained,LABEL_STRING_DELIMITER);
  for(vector<string>::iterator it = subdir_ids.begin() ; it!=subdir_ids.end() ; ++it) {
    string subdir_id = (*it);
    DataHandler::dir_info_st subdir_info;
    RequestDispatcher::info_element_st info_element;
    
    if( !get_directory_info(user_id,user_token,subdir_id,subdir_info,status) ){ return false; }
    info_element.id = stoul(subdir_id,nullptr,10);
    info_element.lastModDate = subdir_info.date_last_mod;
    info_element.name = subdir_info.name;
    info_element.type = LABEL_D;  
    info_element.shared = LABEL_FALSE; // Directory is always shared==false 
    info_element.size = stoul(subdir_info.size,nullptr,10);
    
    // Calculate number of items
    vector<string> temp_subdir_ids = split_string(subdir_info.directories_contained,LABEL_STRING_DELIMITER);
    vector<string> temp_file_ids = split_string(subdir_info.files_contained,LABEL_STRING_DELIMITER);
    info_element.number_of_items = temp_subdir_ids.size() + temp_file_ids.size();
    
    directory_element_info.push_back(info_element);
  }  
  
  
  // Fill file info  
  vector<string> file_ids = split_string(dir_info.files_contained,LABEL_STRING_DELIMITER);  
  for(vector<string>::iterator it = file_ids.begin() ; it!=file_ids.end() ; ++it) {
    string file_id = (*it);
    DataHandler::file_info_st file_info;
    RequestDispatcher::info_element_st info_element;
    
    if( !get_file_info(user_id,user_token,file_id,file_info,status) ){ return false; }
    info_element.id = stoul(file_id,nullptr,10);
    info_element.lastModDate = file_info.date_last_mod;
    info_element.name = file_info.name;
    info_element.type = LABEL_A;  
    info_element.size = stoul(file_info.size,nullptr,10);
    info_element.number_of_items = 0; // File is always number_of_items==0 
    
    // Calculate number of users shared
    vector<string> temp_users_shared = split_string(file_info.users_shared,LABEL_STRING_DELIMITER);
    size_t number_of_users_shared = temp_users_shared.size();
    if(number_of_users_shared > 0){
      info_element.shared = LABEL_TRUE; 
    }else{
      info_element.shared = LABEL_FALSE; 
    }
    
    
    directory_element_info.push_back(info_element);
  }  

  
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


bool RequestDispatcher::change_dir_date_recursive(string dir_id, string new_date, int& status){
  DataHandler::dir_info_st dir_info;
  if( !dh_.get_directory_info(dir_id,dir_info,status) ){ return false; }

  // Recursive
  if( dir_info.parent_directory!=LABEL_NO_PARENT_DIR ){
    if( !change_dir_date_recursive(dir_info.parent_directory,new_date,status) ){ return false; }
  }
  
  return( dh_.modify_directory_info(dir_id,dir_info.name,new_date,dir_info.tags,dir_info.size,status) );
}


bool RequestDispatcher::increase_dir_size_recursive(string dir_id, string size_increased, int& status){
  DataHandler::dir_info_st dir_info;
  if( !dh_.get_directory_info(dir_id,dir_info,status) ){ return false; }

  // Recursive
  if( dir_info.parent_directory!=LABEL_NO_PARENT_DIR ){
    if( !increase_dir_size_recursive(dir_info.parent_directory,size_increased,status) ){ return false; }
  }
  
  size_t new_size = stoul(dir_info.size,nullptr,10) + stoul(size_increased,nullptr,10) ;
  return( dh_.modify_directory_info(dir_id,dir_info.name,dir_info.date_last_mod,dir_info.tags,to_string(new_size),status) );
}


bool RequestDispatcher::decrease_dir_size_recursive(string dir_id, string size_decreased, int& status){
  DataHandler::dir_info_st dir_info;
  if( !dh_.get_directory_info(dir_id,dir_info,status) ){ return false; }

  // Recursive
  if( dir_info.parent_directory!=LABEL_NO_PARENT_DIR ){
    if( !decrease_dir_size_recursive(dir_info.parent_directory,size_decreased,status) ){ return false; }
  }
  
  size_t new_size = stoul(dir_info.size,nullptr,10) - stoul(size_decreased,nullptr,10) ;  
  return( dh_.modify_directory_info(dir_id,dir_info.name,dir_info.date_last_mod,dir_info.tags,to_string(new_size),status) );
}


bool RequestDispatcher::get_root_dir_id(string user_id, string& root_dir_id, int& status){
  DataHandler::user_info_st user_info;
  if( !dh_.get_user_info(user_id,user_info,status) ){ return false; }
  root_dir_id = user_info.dir_root;
  return true;
}


bool RequestDispatcher::db_is_initiated(){
  return init_ok_;
}
