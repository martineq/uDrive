#include "request_dispatcher.h"


RequestDispatcher* RequestDispatcher::request_dispatcher_instance = nullptr;


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


bool RequestDispatcher::sign_up(string email, string password, string first_name, string last_name, string gps_lat, string gps_lon,
                                string new_token, string date, string& user_id, int& status){
  string name = first_name + LABEL_STRING_DELIMITER + last_name;
  string location = gps_lat + LABEL_STRING_DELIMITER + gps_lon;
  return dh_.add_user(email,password,name,location,new_token,date,user_id,status);
}


bool RequestDispatcher::log_in(string email, string password, string new_token, string& user_id, int& status){
  string saved_password;
  if( !dh_.get_user_password(email,saved_password,status) ){ return false; };

  if( password!=saved_password ){ status = STATUS_WRONG_PASSWORD; return false; }

  return dh_.add_user_token(email,new_token,user_id,status);
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


bool RequestDispatcher::new_directory(string user_id, string name, string date, string parent_dir_id,
                                      string& dir_id, int& status){
  if( parent_dir_id==LABEL_ZERO ){
    if( !get_root_dir_id(user_id,parent_dir_id,status) ){ return false; }
  }
  
  if( !dh_.add_directory(user_id,name,date,parent_dir_id,dir_id,status) ){ return false; }
  
  // Change parent_dir_id date
  if( !change_dir_date_recursive(parent_dir_id,date,status) ){ return false; }
  
  return true;
}


bool RequestDispatcher::new_file(string user_id, string name, string extension, string date,
                                 const char* p_file_stream, string size, string parent_dir_id, string& file_id, int& status){
  // Check user quota
  string quota_used;
  if( !get_user_quota_used(user_id,quota_used,status) ){ return false; }
  if( ( stoul_decimal(quota_used) + stoul_decimal(size) ) > max_user_quota_ ){
    status = STATUS_MAX_QUOTA_EXCEEDED;
    return false;
  }

  if( parent_dir_id==LABEL_ZERO ){
    if( !get_root_dir_id(user_id,parent_dir_id,status) ){ return false; }
  }
    
  // Add logical file
  if( !dh_.add_file(user_id,name,extension,date,size,LABEL_REVISION_1,parent_dir_id,file_id,status) ){ return false; }
  
  // Add physical file
  string file_name = user_id+file_id+LABEL_REVISION_1;
  if( fh_.save_file(file_name,p_file_stream,stoul_decimal(size))==0 ){
    if( !dh_.delete_file(file_id,status) ){ return false; }
    status = STATUS_FAIL_SAVING_FILE; return false;
  }

  // Add size to user quota
  if( !increase_user_quota_used(user_id,size,status) ){ return false; }
  
  // Change parent_dir_id date & add size
  if( !change_dir_date_recursive(parent_dir_id,date,status) ){ return false; }

  // Recursive size add
  if( !increase_dir_size_recursive(parent_dir_id,size,status) ){ return false; }
  
  return true;
}


bool RequestDispatcher::get_user_info(string user_id, RequestDispatcher::user_info_st& user_info, int& status){
  
  DataHandler::user_info_st dh_user_info;
  if( !dh_.get_user_info(user_id,dh_user_info,status) ){ return false; }

  // Prepare data
  user_info.email = dh_user_info.email;
  vector<string> name = split_string(dh_user_info.name,LABEL_STRING_DELIMITER);
  user_info.first_name = name.front();
  user_info.last_name = name.back();
  vector<string> location = split_string(dh_user_info.location,LABEL_STRING_DELIMITER);
  user_info.gps_lat = location.front();
  user_info.gps_lon = location.back();
  user_info.user_quota_used = dh_user_info.user_quota_used;
  user_info.user_quota_total = to_string(max_user_quota_);
  float percentage = (stoul_decimal(dh_user_info.user_quota_used) * 100) / max_user_quota_;
  std::string str_percentage = to_string(percentage);
  user_info.user_quota_used_percentage = str_percentage.substr(0,5) + "%";

  return true;
}

bool RequestDispatcher::get_user_image(string user_id, char*& p_image_stream, string& size_stream, int& status){

  string user_image_name = LABEL_USER_IMAGE + user_id;
  size_t size = fh_.load_file(user_image_name,p_image_stream);
  size_stream = to_string(size);
  if( size==0 ){ status = STATUS_FAIL_LOADING_FILE; return false; }
  
  return true;
}



bool RequestDispatcher::get_directory_info(string user_id, string dir_id, RequestDispatcher::dir_info_st& dir_info, int& status){
  
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
  dir_info.date_last_mod = dir_info_temp.date_last_mod;
  dir_info.directories_contained = dir_info_temp.directories_contained;
  dir_info.files_contained = dir_info_temp.files_contained;
  dir_info.name = dir_info_temp.name;
  dir_info.owner = dir_info_temp.owner;
  dir_info.parent_directory = dir_info_temp.parent_directory;
  dir_info.size = dir_info_temp.size;
  dir_info.tags = dir_info_temp.tags;
  
  
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


bool RequestDispatcher::get_directory_element_info_from_dir_info(string user_id, RequestDispatcher::dir_info_st dir_info,
                                                                 vector< RequestDispatcher::info_element_st >& directory_element_info, int& status){
  directory_element_info.clear();
  
  // Fill subdir info
  vector<string> subdir_ids = split_string(dir_info.directories_contained,LABEL_STRING_DELIMITER);
  for(vector<string>::iterator it = subdir_ids.begin() ; it!=subdir_ids.end() ; ++it) {
    string subdir_id = (*it);
    RequestDispatcher::dir_info_st subdir_info;
    RequestDispatcher::info_element_st info_element;
    
    if( !get_directory_info(user_id,subdir_id,subdir_info,status) ){ return false; }
    info_element.id = stoul_decimal(subdir_id);
    info_element.lastModDate = subdir_info.date_last_mod;
    info_element.name = subdir_info.name;
    info_element.type = LABEL_D;  
    info_element.shared = LABEL_FALSE; // Directory is always shared==false 
    info_element.size = stoul_decimal(subdir_info.size);
    
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
    RequestDispatcher::file_info_st file_info;
    RequestDispatcher::info_element_st info_element;
    
    if( !get_file_info(user_id,file_id,file_info,status) ){ return false; }
    info_element.id = stoul_decimal(file_id);
    info_element.lastModDate = file_info.date_last_mod;
    info_element.name = file_info.name;
    info_element.type = LABEL_A;  
    info_element.size = stoul_decimal(file_info.size);
    info_element.number_of_items = 0; // File is always number_of_items==0 
    
    // Calculate number of users shared
    vector<string> temp_users_shared = split_string(file_info.users_shared,LABEL_STRING_DELIMITER);
    size_t number_of_users_shared = temp_users_shared.size();
    if(number_of_users_shared > 0){ info_element.shared = LABEL_TRUE; 
    }else{ info_element.shared = LABEL_FALSE; }
    
    directory_element_info.push_back(info_element);
  }  

  
  return true;
}


bool RequestDispatcher::get_file_info(string user_id, string file_id, RequestDispatcher::file_info_st& file_info, int& status){

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

  // Assign file_info value
  file_info.date_last_mod = file_info_temp.date_last_mod;
  file_info.deleted_status = file_info_temp.deleted_status;
  file_info.extension = file_info_temp.extension;
  file_info.name = file_info_temp.name;
  file_info.owner = file_info_temp.owner;
  file_info.revision = file_info_temp.revision;
  file_info.size = file_info_temp.size;
  file_info.tags = file_info_temp.tags;
  file_info.user_last_mod = file_info_temp.user_last_mod;
  file_info.users_shared = file_info_temp.users_shared;
  
  return true;
}


bool RequestDispatcher::get_file_stream(string user_id, string file_id, string revision,
                                        char*& p_file_stream, string& size_stream, int& status){
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

  string file_name = user_id+file_id+LABEL_REVISION_1;
  size_t size = fh_.load_file(file_name,p_file_stream);
  size_stream = to_string(size);
  if( size==0 ){ status = STATUS_FAIL_LOADING_FILE; return false; }

  return true;
}


bool RequestDispatcher::get_dir_stream(string user_id, string dir_id,
                                       char*& p_dir_stream, string& size_stream, int& status){
  
  // Gets structure info for the directory
  ZipHandler::dir_tree_node_st dir_structure = get_dir_structure_recursive(user_id,dir_id,status);
  
  // Generate the zip file
  string zip_name = "zipfile_" + user_id;
  zh_.create_zip(zip_name,dir_structure);
  
  // Load file in p_dir_stream
  string zip_name_with_extension = zip_name + ".zip";
  size_t size = fh_.load_file(zip_name_with_extension,p_dir_stream);
  size_stream = to_string(size);
  if( size==0 ){ status = STATUS_FAIL_LOADING_FILE; return false; }
  
  return true;
}


bool RequestDispatcher::set_user_image(string user_id, const char* p_image_stream, string size, int& status){

  string user_image_name = LABEL_USER_IMAGE + user_id;
  if( fh_.save_file(user_image_name,p_image_stream,stoul_decimal(size))==0 ){ status = STATUS_FAIL_SAVING_FILE; return false; }

  return true;
}


/*
bool RequestDispatcher::delete_user(string user_id, int& status){
  // TODO(mart): implement
//+ Borrar usuario -> delete_user(). Verificar que el usr sea dueño. ¿Borrar todos sus archivos y carpetas?
  return false;
}


bool RequestDispatcher::delete_directory(string user_id, string dir_id, int& status){
  // TODO(mart): implement
// + Borrar directorio -> delete_directory(). Verificar que el usr sea dueño. Borrar todas las subcarpetas y archivos que contenga.
  return false;
}


bool RequestDispatcher::delete_file(string user_id, string file_id, int& status){
  // TODO(mart): implement
//+ Borrar archivo ->  delete_file(). En caso de ser dueño, verificar si el archivo está compartido y quitar ese estado a los demás usuarios. En caso de ser compartido sólo quitarse de la lista de compartidos.
  return false;
}
*/


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
  
  size_t new_quota = stoul_decimal(user_info.user_quota_used) + stoul_decimal(quota_increased);
  
  return( dh_.modify_user_info(user_id,user_info.email,user_info.name,user_info.location,
                               user_info.shared_files,to_string(new_quota),status) );
}


bool RequestDispatcher::decrease_user_quota_used(string user_id, string quota_decreased, int& status){
  DataHandler::user_info_st user_info;
  if( !dh_.get_user_info(user_id,user_info,status) ){ return false; }
  
  size_t new_quota = stoul_decimal(user_info.user_quota_used) - stoul_decimal(quota_decreased);
  
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
  
  size_t new_size = stoul_decimal(dir_info.size) + stoul_decimal(size_increased);
  return( dh_.modify_directory_info(dir_id,dir_info.name,dir_info.date_last_mod,dir_info.tags,to_string(new_size),status) );
}


bool RequestDispatcher::decrease_dir_size_recursive(string dir_id, string size_decreased, int& status){
  DataHandler::dir_info_st dir_info;
  if( !dh_.get_directory_info(dir_id,dir_info,status) ){ return false; }

  // Recursive
  if( dir_info.parent_directory!=LABEL_NO_PARENT_DIR ){
    if( !decrease_dir_size_recursive(dir_info.parent_directory,size_decreased,status) ){ return false; }
  }
  
  size_t new_size = stoul_decimal(dir_info.size) - stoul_decimal(size_decreased);  
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


ZipHandler::dir_tree_node_st RequestDispatcher::get_dir_structure_recursive(string user_id, string dir_id, int& status){

  // Create node
  ZipHandler::dir_tree_node_st node;
  node.dir_node_name.clear();
  node.files_contained.clear();
  node.sub_dirs.clear();
  
  // Get directory info
  RequestDispatcher::dir_info_st dir_info;
  if( !get_directory_info(user_id,dir_id,dir_info,status) ){ return node; }

  // Fill dir_node_name;
  node.dir_node_name = dir_info.name;  
  
  // Fill files_contained
  vector<string> file_ids = split_string(dir_info.files_contained,LABEL_STRING_DELIMITER);  
  for(vector<string>::iterator it = file_ids.begin() ; it!=file_ids.end() ; ++it) {
    string file_id = (*it);
    RequestDispatcher::file_info_st file_info;
    if( !get_file_info(user_id,file_id,file_info,status) ){ return node; }
    ZipHandler::file_st file_zip_info;
    file_zip_info.file_id = user_id + file_id + file_info.revision;
    file_zip_info.file_real_name = file_info.name + "." + file_info.extension;
    node.files_contained.push_back(file_zip_info);
  }
  
  //Fill sub_dirs recursively
  vector<string> subdir_ids = split_string(dir_info.directories_contained,LABEL_STRING_DELIMITER);
  for(vector<string>::iterator it = subdir_ids.begin() ; it!=subdir_ids.end() ; ++it) {
    string subdir_id = (*it);
    ZipHandler::dir_tree_node_st child_dir_node = get_dir_structure_recursive(user_id,subdir_id,status); // Recursive
    node.sub_dirs.push_back(child_dir_node);
  }  
  
  return node;
}


long unsigned int RequestDispatcher::stoul_decimal(const string& str){
  return stoul(str,nullptr,10);
}

