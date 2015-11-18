#include "request_dispatcher.h"


RequestDispatcher* RequestDispatcher::request_dispatcher_instance_ = nullptr;


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


bool RequestDispatcher::log_in(string email, string password, string new_token, string& user_id, string& quota_available , int& status){
  string saved_password;
  if( !dh_.get_user_password(email,saved_password,status) ){ return false; }

  if( password!=saved_password ){ status = STATUS_WRONG_PASSWORD; return false; }

  if( !dh_.add_user_token(email,new_token,user_id,status) ){ return false; }
  
  DataHandler::user_info_st user_info;
  if( !dh_.get_user_info(user_id,user_info,status) ){ return false; }
  quota_available = to_string( max_user_quota_ - stoul_decimal(user_info.user_quota_used) );
  
  return true; 
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
  
  // Search for a existent file revision in the same directory
  bool revision_found = false;
  string revision_file_id = "0";
  DataHandler::file_info_st revision_file_info;
  if( !search_revision_file(parent_dir_id,name,extension,revision_found,revision_file_id,revision_file_info,status) ){ return false; }
  
  // Case: Existent file revision
  if( revision_found ){  
    // Add physical file revision
    string new_revision = to_string( stoul_decimal(revision_file_info.revision) + 1 );
    string file_name = user_id+file_id+new_revision;
    if( fh_.save_file(file_name,p_file_stream,stoul_decimal(size))==0 ){ status = STATUS_FAIL_SAVING_FILE; return false; }
    // Update information of file revision
    if( !increase_file_revision(revision_file_id,status) ){ return false; }
  }
  
  // Case: New file (no previous file revision)
  if( !revision_found ){
     // Add logical file
    if( !dh_.add_file(user_id,name,extension,date,size,LABEL_REVISION_1,parent_dir_id,file_id,status) ){ return false; }
    // Add physical file
    string file_name = user_id+file_id+LABEL_REVISION_1;
    if( fh_.save_file(file_name,p_file_stream,stoul_decimal(size))==0 ){
      if( !dh_.delete_file(file_id,status) ){ return false; }  status = STATUS_FAIL_SAVING_FILE; return false;
    }
  }  

  // Add size to user quota
  if( !increase_user_quota_used(user_id,size,status) ){ return false; }

  // Change parent_dir_id date
  if( !change_dir_date_recursive(parent_dir_id,date,status) ){ return false; }

  // If there is a previous revision, decrease directory size for the old revision
  if( revision_found ){ if( !decrease_dir_size_recursive(parent_dir_id,revision_file_info.size,status) ){ return false; } } 
  
  // Recursive size add to directory
  if( !increase_dir_size_recursive(parent_dir_id,size,status) ){ return false; }
    
  return true;
}


bool RequestDispatcher::get_user_info(string user_id, RequestDispatcher::user_info_st& user_info, int& status){
  
  DataHandler::user_info_st dh_user_info;
  if( !dh_.get_user_info(user_id,dh_user_info,status) ){ return false; }

  // Prepare data
  fill_user_info_st(user_id,dh_user_info,user_info);
  
  return true;
}


bool RequestDispatcher::get_user_image(string user_id, string& image_stream, int& status){
  char* p_image_stream = nullptr;
  string user_image_name = LABEL_USER_IMAGE + user_id;
  size_t size = fh_.load_file(user_image_name,p_image_stream);
  if( size==0 ){ image_stream.clear(); status = STATUS_FAIL_LOADING_FILE; return false; }
  string img_str(p_image_stream,size);
  image_stream = img_str;
  delete p_image_stream;
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
  vector<RequestDispatcher::info_element_st> directory_element_info;
  if( !get_directory_element_info_from_dir_info(dir_info_temp,directory_element_info,status)){ return false; }
  
  dir_info.directory_element_info = directory_element_info;
  dir_info.date_last_mod = dir_info_temp.date_last_mod;
  dir_info.name = dir_info_temp.name;
  dir_info.owner = dir_info_temp.owner;
  dir_info.parent_directory = dir_info_temp.parent_directory;
  dir_info.size = dir_info_temp.size;
  dir_info.tags = dir_info_temp.tags;
  dir_info.owner = dir_info_temp.owner;
  
  if( !is_root_dir ){
    // Check if the parent dir is the root dir, and then, format the root dir ID (change to id==0)
    DataHandler::dir_info_st parent_dir_info_temp;
    if( !dh_.get_directory_info(dir_info.parent_directory,parent_dir_info_temp,status) ){ return false; }
    if( parent_dir_info_temp.parent_directory == LABEL_NO_PARENT_DIR ){ dir_info.parent_directory = LABEL_ZERO; }   
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
  file_info.extension = file_info_temp.extension;
  file_info.name = file_info_temp.name;
  file_info.revision = file_info_temp.revision;
  file_info.size = file_info_temp.size;
  file_info.tags = file_info_temp.tags;
  file_info.user_last_mod = file_info_temp.user_last_mod;
  file_info.parent_directory = file_info_temp.parent_directory;
  
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
  
  if( !user_authorized ){ status = STATUS_USER_FORBIDDEN; return false; }

  if( stoul_decimal(file_info_temp.revision) < stoul_decimal(revision) ){ status = STATUS_REVISION_NOT_EXISTS; return false; }
  
  string file_name = file_info_temp.owner+file_id+revision;
  size_t size = fh_.load_file(file_name,p_file_stream);
  size_stream = to_string(size);
  if( size==0 ){ status = STATUS_FAIL_LOADING_FILE; return false; }

  return true;
}


bool RequestDispatcher::get_dir_stream(string user_id, string dir_id,
                                       char*& p_dir_stream, string& size_stream, int& status){

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
  
  // Gets structure info for the directory
  ZipHandler::dir_tree_node_st dir_structure = get_dir_structure_recursive(dir_id,status);
  
  // Generate the zip file
  string zip_name = "zipfile_" + user_id;
  zh_.create_zip(zip_name,dir_structure);

  // Load file in p_dir_stream
  string zip_name_with_extension = zip_name + ".zip";
  size_t size = fh_.load_file(zip_name_with_extension,p_dir_stream);
  size_stream = to_string(size);
  if( size==0 ){ status = STATUS_FAIL_LOADING_FILE; return false; }
  
  // Delete physical zip file
  fh_.delete_file(zip_name_with_extension);
  
  return true;
}


bool RequestDispatcher::get_shared_files(string user_id, vector<RequestDispatcher::info_element_st>& shared_files, int& status){

  DataHandler::user_info_st dh_user_info;
  if( !dh_.get_user_info(user_id,dh_user_info,status) ){ return false; }

  // Prepare data
  shared_files.clear();
  if( !add_info_files_from_id_list(dh_user_info.shared_files,SEARCH_MODE_OFF,LABEL_EMPTY_STRING,shared_files,status) ){ return false; }
  
  return true;
}


bool RequestDispatcher::get_deleted_files(string user_id, vector< RequestDispatcher::info_element_st >& deleted_files, int& status){

  DataHandler::user_info_st dh_user_info;
  if( !dh_.get_user_info(user_id,dh_user_info,status) ){ return false; }

  // Prepare data
  deleted_files.clear();
  if( !add_info_files_from_id_list(dh_user_info.files_deleted,SEARCH_MODE_OFF,LABEL_EMPTY_STRING,deleted_files,status) ){ return false; }  
  
  return true;
}


bool RequestDispatcher::get_tags(string user_id, vector<string>& tags, int& status){

  DataHandler::user_info_st dh_user_info;
  if( !dh_.get_user_info(user_id,dh_user_info,status) ){ return false; }

  // Add tags from files and directories from the user
  if( !add_tags_recursive(dh_user_info.dir_root,tags,status) ){ return false; }
  
  // Add tags from shared files
  if( !add_tags_from_id_list(dh_user_info.shared_files,tags,status) ){ return false; }
  
  return true;
}


bool RequestDispatcher::get_user_email_list_full(vector<string>& list, int& status){
  string str_list;
  list.clear();
  if( !dh_.get_user_mail_list(str_list,status) ){ return false; }
  list = split_string(str_list,LABEL_STRING_DELIMITER);
  return true;
}


bool RequestDispatcher::get_user_email_list_by_pattern(string pattern, vector<string>& list, int& status){
  vector<string> full_list;
  list.clear();
  if( !get_user_email_list_full(full_list,status) ){ return false; }
  for(vector<string>::iterator it = full_list.begin() ; it!=full_list.end() ; ++it) {
      if( (*it).find(pattern)!=string::npos ){ list.push_back((*it)); }
  }
  return true;
}


bool RequestDispatcher::get_colaborator_users(string user_id, vector< RequestDispatcher::user_info_st >& users_founded, int& status){

  users_founded.clear();
  DataHandler::user_info_st owner_info;
  if( !dh_.get_user_info(user_id,owner_info,status) ){ return false; }
  
  // Search colaborators users in shared files
  vector<string> file_ids = split_string(owner_info.shared_files,LABEL_STRING_DELIMITER);
  string ids_of_colaborators;
  for(vector<string>::iterator it = file_ids.begin() ; it!=file_ids.end() ; ++it) {
    string file_id = (*it);
    DataHandler::file_info_st file_info;
    if( !dh_.get_file_info(file_id,file_info,status) ){ return false; }
    ids_of_colaborators = add_key_to_string_list(ids_of_colaborators,file_info.owner);
  } 
  
  // Fill all data of users founded
  vector<string> colaborators_ids = split_string(ids_of_colaborators,LABEL_STRING_DELIMITER);
  for(vector<string>::iterator it = colaborators_ids.begin() ; it!=colaborators_ids.end() ; ++it) {
    string user_id = (*it);
    RequestDispatcher::user_info_st user_info;
    if( !get_user_info(user_id,user_info,status) ){ return false; }
    users_founded.push_back(user_info);
  }

  return true;
}


bool RequestDispatcher::set_user_image(string user_id, const char* p_image_stream, string size, int& status){

  string user_image_name = LABEL_USER_IMAGE + user_id;
  if( fh_.save_file(user_image_name,p_image_stream,stoul_decimal(size))==0 ){ status = STATUS_FAIL_SAVING_FILE; return false; }

  return true;
}


bool RequestDispatcher::set_file_share(string user_owner_id, string file_id, string user_shared_id, string date, int& status){
  
  DataHandler::file_info_st file_info;
  if( !dh_.get_file_info(file_id,file_info,status) ){ return false; }

  // Search for authorized user
  bool user_authorized = (file_info.owner.compare(user_owner_id)==0);
  if( !user_authorized ){ status = STATUS_USER_FORBIDDEN; return false; }

  // Add file to user shared
  DataHandler::user_info_st user_shared_info;
  if( !dh_.get_user_info(user_shared_id,user_shared_info,status)){ return false; }
  user_shared_info.shared_files = add_key_to_string_list(user_shared_info.shared_files,file_id);
  bool ok = dh_.modify_user_info(user_shared_id,user_shared_info.email,user_shared_info.name,user_shared_info.location,
                                 user_shared_info.shared_files,user_shared_info.user_quota_used,
                                 user_shared_info.files_deleted,status);
  if( !ok ){ return false; }
  
  // Add user shared to file
  file_info.users_shared = add_key_to_string_list(file_info.users_shared,user_shared_id);
  ok = dh_.modify_file_info(file_id,file_info.name,file_info.extension,date,file_info.tags,
                            file_info.users_shared,user_owner_id,file_info.parent_directory,status);
  if( !ok ){ return false; }
  
  return true;
}


bool RequestDispatcher::unset_file_share(string user_owner_id, string file_id, string user_shared_id, string date, int& status){

  DataHandler::file_info_st file_info;
  if( !dh_.get_file_info(file_id,file_info,status) ){ return false; }

  // Search for authorized user
  bool user_authorized = (file_info.owner.compare(user_owner_id)==0);
  if( !user_authorized ){ status = STATUS_USER_FORBIDDEN; return false; }

  // Remove file from user shared
  DataHandler::user_info_st user_shared_info;
  if( !dh_.get_user_info(user_shared_id,user_shared_info,status)){ return false; }
  user_shared_info.shared_files = remove_key_from_string_list(user_shared_info.shared_files,file_id);
  bool ok = dh_.modify_user_info(user_shared_id,user_shared_info.email,user_shared_info.name,
                       user_shared_info.location,user_shared_info.shared_files,user_shared_info.user_quota_used,
                       user_shared_info.files_deleted,status);
  if( !ok ){ return false; }
  
  // Remove user shared from file
  file_info.users_shared = remove_key_from_string_list(file_info.users_shared,user_shared_id);
  ok = dh_.modify_file_info(file_id,file_info.name,file_info.extension,date,file_info.tags,
                            file_info.users_shared,user_owner_id,file_info.parent_directory,status);
  if( !ok ){ return false; }
  
  return true;
}


bool RequestDispatcher::modify_user_info(string user_id, string email, string first_name, string last_name, string gps_lat, string gps_lon, int& status){
  
  DataHandler::user_info_st user_info;
  if( !dh_.get_user_info(user_id,user_info,status) ){ return false; }

  string name = first_name + LABEL_STRING_DELIMITER + last_name;
  string location = gps_lat + LABEL_STRING_DELIMITER + gps_lon;

  
  if( !dh_.modify_user_info(user_id,email,name,location,user_info.shared_files,
    user_info.user_quota_used,user_info.files_deleted,status) ){ return false; }
    
  return true;
}


bool RequestDispatcher::modify_user_password(string user_id, string password, int& status){
  return dh_.modify_user_password(user_id,password,status);
}


bool RequestDispatcher::modify_directory_info(string user_id, string dir_id, string name, string date, string tags, int& status){
  
  bool is_root_dir = (dir_id==LABEL_ZERO);
  
  // The user cant modify info for the root directory
  if( is_root_dir ){ status = STATUS_DELETE_ROOT_DIR_FORBIDDEN; return false; }
  
  DataHandler::dir_info_st dir_info;
  if( !dh_.get_directory_info(dir_id,dir_info,status) ){ return false; }
  
  if( user_id!=dir_info.owner ){ status = STATUS_USER_FORBIDDEN; return false; }

  if( !dh_.modify_directory_info(dir_id,name,date,tags,dir_info.size,status) ){ return false; }
     
  return true;
}


bool RequestDispatcher::modify_file_info(string user_id, string file_id, string name, string extension, string date, string tags, int& status){

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
  
  if( !user_authorized ){ status = STATUS_USER_FORBIDDEN; return false; }

  if( !dh_.modify_file_info(file_id,name,extension,date,tags,file_info_temp.users_shared,user_id,file_info_temp.parent_directory,status)){ return false; }
  
  return true;
}


bool RequestDispatcher::delete_user(string user_id, int& status){
  
  DataHandler::user_info_st user_info;
  if( !dh_.get_user_info(user_id,user_info,status) ){ return false; }
  
  // Delete root directory, and their sub-directories and files contained
  if( !delete_dir_recursive(user_info.dir_root,status) ){ return false; }
  
  // Delete physical of the files deleted logically
  if( !purge_deleted_files(user_id,status) ){ return false; }
  
  // Delete info of the user
  if( !dh_.delete_user(user_id,status) ){ return false; }
  
  return true;
}


bool RequestDispatcher::delete_directory(string user_id, string dir_id, int& status){
  
  bool is_root_dir = (dir_id==LABEL_ZERO);
  
  // The user cant delete the root directory
  if( is_root_dir ){ status = STATUS_DELETE_ROOT_DIR_FORBIDDEN; return false; }
  
  DataHandler::dir_info_st dir_info;
  if( !dh_.get_directory_info(dir_id,dir_info,status) ){ return false; }
  
  if( user_id!=dir_info.owner ){ status = STATUS_USER_FORBIDDEN; return false; }

  // Delete directory, and their sub-directories and files contained
  if( !delete_dir_recursive(dir_id,status) ){ return false; }
  
  // Delete link with their parent directory
  DataHandler::dir_info_st parent_dir_info;
  if( !dh_.get_directory_info(dir_info.parent_directory,parent_dir_info,status) ){ return false; }
  parent_dir_info.directories_contained = remove_key_from_string_list(parent_dir_info.directories_contained,dir_id);
  if( !dh_.modify_directory_dirs_contained(dir_info.parent_directory,parent_dir_info.directories_contained,status) ){ return false; }
  
  return true;
}


bool RequestDispatcher::delete_file(string user_id, string file_id, int& status){
  
  DataHandler::file_info_st file_info;
  if( !dh_.get_file_info(file_id,file_info,status) ){ return false; }

  // Search for authorized user
  bool user_is_owner = (file_info.owner.compare(user_id)==0);
  bool user_is_shared = false;
  if( !user_is_owner ){
    vector<string> users_authorized = split_string(file_info.users_shared,LABEL_STRING_DELIMITER);
    for(vector<string>::iterator it = users_authorized.begin() ; ( it!=users_authorized.end() && !user_is_shared ) ; ++it) {
      if( (*it).compare(user_id)==0 ){ user_is_shared = true; }
    }
  }
  
  if( !user_is_owner && !user_is_shared ){ status = STATUS_USER_FORBIDDEN; return false; }

  // If user is the owner: delete the file (move from parent_dir to the user "files_deleted"), and unshare from all other users
  if( user_is_owner ){

    // Copy file_id to files_deleted
    DataHandler::user_info_st owner_info;
    if( !dh_.get_user_info(user_id,owner_info,status) ){ return false; }
    owner_info.files_deleted = add_key_to_string_list(owner_info.files_deleted,LABEL_STRING_DELIMITER+file_id);
    if( !dh_.modify_user_info(user_id,owner_info.email,owner_info.name,owner_info.location,
      owner_info.shared_files,owner_info.user_quota_used,owner_info.files_deleted,status) ){ return false; }
    
    // Delete file_id from parent_dir.files_contained
    DataHandler::dir_info_st parent_dir_info;
    if( !dh_.get_directory_info(file_info.parent_directory,parent_dir_info,status) ){ return false; }
    string new_files_contained = remove_key_from_string_list(parent_dir_info.files_contained,file_id);
    if( !dh_.modify_directory_files_contained(file_info.parent_directory,new_files_contained,status) ){ return false; }
    
    // Unset file share for all users shared of this file
    vector<string> users_shared = split_string(file_info.users_shared,LABEL_STRING_DELIMITER);
    for(vector<string>::iterator it = users_shared.begin() ; it!=users_shared.end() ; ++it) {
      string user_shared_id = (*it);
      if( !unset_file_share(user_id,file_id,user_shared_id,file_info.date_last_mod,status) ){ return false; }
    }
    
    // Change size to directory
    if( !decrease_dir_size_recursive(file_info.parent_directory,file_info.size,status) ){ return false; }

  }

  // If user is a shared user: unshare the file only from him (dont move the file)
  if( user_is_shared ){
    if( !unset_file_share(file_info.owner,file_id,user_id,file_info.date_last_mod,status) ){ return false; }
  }

  return true;
}


bool RequestDispatcher::purge_deleted_files(string user_id, int& status){

  // Get owner info
  DataHandler::user_info_st owner_info;
  if( !dh_.get_user_info(user_id,owner_info,status) ){ return false; }

  // Delete files
  if( !purge_deleted_files_from_user_info(owner_info,status) ){ return false; }

  // Delete all owner_info.files_deleted
  if( !dh_.modify_user_info(user_id,owner_info.email,owner_info.name,owner_info.location,
    owner_info.shared_files,owner_info.user_quota_used,LABEL_EMPTY_STRING,status) ){ return false; }

  return true;
}


bool RequestDispatcher::purge_deleted_files(string user_id, vector<string> selected_files_id, int& status){

  // Get owner info
  DataHandler::user_info_st owner_info;
  if( !dh_.get_user_info(user_id,owner_info,status) ){ return false; }

  // Select files to recover
  string files_to_purge;
  string files_untouched;
  divide_selected_ids(owner_info.files_deleted,selected_files_id,files_to_purge,files_untouched);

  // Delete files
  owner_info.files_deleted = files_to_purge; // Add only selected files
  if( !purge_deleted_files_from_user_info(owner_info,status) ){ return false; }

  // Delete all owner_info.files_deleted
  if( !dh_.modify_user_info(user_id,owner_info.email,owner_info.name,owner_info.location,
    owner_info.shared_files,owner_info.user_quota_used,files_untouched,status) ){ return false; }
  
  
  return true;
}


bool RequestDispatcher::recover_deleted_files(string user_id, int& status){
  
  // Get owner info
  DataHandler::user_info_st owner_info;
  if( !dh_.get_user_info(user_id,owner_info,status) ){ return false; }
  
  // Recover files
  if( !recover_deleted_files_from_user_info(owner_info,status) ){ return false; }

  // Delete all owner_info.files_deleted
  if( !dh_.modify_user_info(user_id,owner_info.email,owner_info.name,owner_info.location,
    owner_info.shared_files,owner_info.user_quota_used,LABEL_EMPTY_STRING,status) ){ return false; }

  return true;
}


bool RequestDispatcher::recover_deleted_files(string user_id, vector<string> selected_files_id, int& status){

  // Get owner info
  DataHandler::user_info_st owner_info;
  if( !dh_.get_user_info(user_id,owner_info,status) ){ return false; }

  // Select files to recover
  string files_to_recover;
  string files_untouched;
  divide_selected_ids(owner_info.files_deleted,selected_files_id,files_to_recover,files_untouched);

  // Recover files
  owner_info.files_deleted = files_to_recover; // Add only selected files
  if( !recover_deleted_files_from_user_info(owner_info,status) ){ return false; }
  
  // Save all file untouched
  if( !dh_.modify_user_info(user_id,owner_info.email,owner_info.name,owner_info.location,
    owner_info.shared_files,owner_info.user_quota_used,files_untouched,status) ){ return false; }

    return true;
}


bool RequestDispatcher::search_by_name(string user_id, string name,
                                       vector< RequestDispatcher::info_element_st >& elements_founded, int& status){
  // Search in owned files
  elements_founded.clear();
  DataHandler::user_info_st owner_info;
  if( !dh_.get_user_info(user_id,owner_info,status) ){ return false; }
  if( !search_by_mode_recursive(owner_info.dir_root,SEARCH_MODE_BY_NAME,name,elements_founded,status) ){ return false; }
  
  // Search in shared files
  if( !add_info_files_from_id_list(owner_info.shared_files,SEARCH_MODE_BY_NAME,name,elements_founded,status) ){ return false; }  

  return true;
}


bool RequestDispatcher::search_by_extension(string user_id, string extension,
                                            vector< RequestDispatcher::info_element_st >& elements_founded, int& status){
  // Search in owned files
  elements_founded.clear();
  DataHandler::user_info_st owner_info;
  if( !dh_.get_user_info(user_id,owner_info,status) ){ return false; }
  if( !search_by_mode_recursive(owner_info.dir_root,SEARCH_MODE_BY_EXTENSION,extension,elements_founded,status) ){ return false; }

  // Search in shared files
  if( !add_info_files_from_id_list(owner_info.shared_files,SEARCH_MODE_BY_EXTENSION,extension,elements_founded,status) ){ return false; }  

  return true;
}


bool RequestDispatcher::search_by_tag(string user_id, string tag,
                                      vector< RequestDispatcher::info_element_st >& elements_founded, int& status){
  // Search in owned files
  elements_founded.clear();
  DataHandler::user_info_st owner_info;
  if( !dh_.get_user_info(user_id,owner_info,status) ){ return false; }
  if( !search_by_mode_recursive(owner_info.dir_root,SEARCH_MODE_BY_TAG,tag,elements_founded,status) ){ return false; }

  // Search in shared files
  if( !add_info_files_from_id_list(owner_info.shared_files,SEARCH_MODE_BY_TAG,tag,elements_founded,status) ){ return false; }  
  
  return true;
}


bool RequestDispatcher::search_by_user(string user_id, string user_id_to_search, vector< RequestDispatcher::info_element_st >& elements_founded, int& status){

  elements_founded.clear();
  DataHandler::user_info_st owner_info;
  if( !dh_.get_user_info(user_id,owner_info,status) ){ return false; }

  // Search in shared files
  if( !add_info_files_from_id_list(owner_info.shared_files,SEARCH_MODE_BY_USER,user_id_to_search,elements_founded,status) ){ return false; }  
  
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
  
  size_t new_quota = stoul_decimal(user_info.user_quota_used) + stoul_decimal(quota_increased);
  
  return( dh_.modify_user_info(user_id,user_info.email,user_info.name,user_info.location,
                               user_info.shared_files,to_string(new_quota),user_info.files_deleted,status) );
}


bool RequestDispatcher::decrease_user_quota_used(string user_id, string quota_decreased, int& status){
  DataHandler::user_info_st user_info;
  if( !dh_.get_user_info(user_id,user_info,status) ){ return false; }
  
  size_t new_quota = stoul_decimal(user_info.user_quota_used) - stoul_decimal(quota_decreased);
  
  return( dh_.modify_user_info(user_id,user_info.email,user_info.name,user_info.location,
                               user_info.shared_files,to_string(new_quota),user_info.files_deleted,status) );
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


ZipHandler::dir_tree_node_st RequestDispatcher::get_dir_structure_recursive(string dir_id, int& status){

  // Create node
  ZipHandler::dir_tree_node_st node;
  node.dir_node_name.clear();
  node.files_contained.clear();
  node.sub_dirs.clear();
  
  // Get directory info
  DataHandler::dir_info_st dir_info;
  if( !dh_.get_directory_info(dir_id,dir_info,status) ){ return node; }

  // Fill dir_node_name;
  node.dir_node_name = dir_info.name;  
  
  // Fill files_contained
  vector<string> file_ids = split_string(dir_info.files_contained,LABEL_STRING_DELIMITER);  
  for(vector<string>::iterator it = file_ids.begin() ; it!=file_ids.end() ; ++it) {
    string file_id = (*it);
    DataHandler::file_info_st file_info;
    if( !dh_.get_file_info(file_id,file_info,status) ){ return node; }
    ZipHandler::file_st file_zip_info;
    file_zip_info.file_id = file_info.owner + file_id + file_info.revision;
    file_zip_info.file_real_name = file_info.name + "." + file_info.extension;
    node.files_contained.push_back(file_zip_info);
  }
  
  //Fill sub_dirs recursively
  vector<string> subdir_ids = split_string(dir_info.directories_contained,LABEL_STRING_DELIMITER);
  for(vector<string>::iterator it = subdir_ids.begin() ; it!=subdir_ids.end() ; ++it) {
    string subdir_id = (*it);
    ZipHandler::dir_tree_node_st child_dir_node = get_dir_structure_recursive(subdir_id,status); // Recursive
    node.sub_dirs.push_back(child_dir_node);
  }  
  
  return node;
}


long unsigned int RequestDispatcher::stoul_decimal(const string& str){
  return stoul(str,nullptr,10);
}


string RequestDispatcher::add_key_to_string_list(string list, string key){
  
  vector<string> list_splited = split_string(list,LABEL_STRING_DELIMITER);
  bool founded = false;
  for(vector<string>::iterator it = list_splited.begin() ; it!=list_splited.end() ; ++it) {
    if( (*it).compare(key)==0 ){ founded = true; }
  }

  if(!founded){
    string new_key = LABEL_STRING_DELIMITER + key;
    list.append(new_key);
  }
  
  return list;
}

string RequestDispatcher::remove_key_from_string_list(string list, string key){
  
  vector<string> list_splited = split_string(list,LABEL_STRING_DELIMITER);
  string new_list;
  for(vector<string>::iterator it = list_splited.begin() ; it!=list_splited.end() ; ++it) {
    if( (*it).compare(key)!=0 ){ new_list.append( LABEL_STRING_DELIMITER + (*it) ); }
  }
  
  return new_list;
}


bool RequestDispatcher::get_directory_element_info_from_dir_info(DataHandler::dir_info_st dir_info,
                                                                 vector<RequestDispatcher::info_element_st>& directory_element_info,
                                                                 int& status){
  directory_element_info.clear();
  
  // Fill subdir info
  if( !add_info_dirs_from_id_list(dir_info.directories_contained,directory_element_info,status) ){ return false; }
  
  // Fill file info  
  if( !add_info_files_from_id_list(dir_info.files_contained,SEARCH_MODE_OFF,LABEL_EMPTY_STRING,directory_element_info,status) ){ return false; }
  
  return true;
}


bool RequestDispatcher::delete_dir_recursive(string dir_id, int& status){
  
  // Get directory info
  DataHandler::dir_info_st dir_info;
  if( !dh_.get_directory_info(dir_id,dir_info,status) ){ return false; }

  // Delete sub_dirs recursively
  vector<string> subdir_ids = split_string(dir_info.directories_contained,LABEL_STRING_DELIMITER);
  for(vector<string>::iterator it = subdir_ids.begin() ; it!=subdir_ids.end() ; ++it) {
    string subdir_id = (*it);
    if( !delete_dir_recursive(subdir_id,status) ){ return false; }
  }
  
  // Delete files_contained
  vector<string> file_ids = split_string(dir_info.files_contained,LABEL_STRING_DELIMITER);  
  for(vector<string>::iterator it = file_ids.begin() ; it!=file_ids.end() ; ++it) {
    string file_id = (*it);
    DataHandler::file_info_st file_info;
    if( !dh_.get_file_info(file_id,file_info,status) ){ return false; }
    if( !delete_file(file_info.owner,file_id,status) ){ return false; }
  }
  
  // Delete dir info
  if( !dh_.delete_directory(dir_id,status) ){ return false; }
  
  return true;
}


bool RequestDispatcher::search_revision_file(string parent_dir_id, string name, string extension, bool &revision_found,
                                             string &revision_file_id, DataHandler::file_info_st &revision_file_info, int status){
  revision_found = false;
  DataHandler::dir_info_st parent_dir_info;
  if( !dh_.get_directory_info(parent_dir_id,parent_dir_info,status) ){ return false; }
  vector<string> existent_files = split_string(parent_dir_info.files_contained,LABEL_STRING_DELIMITER);
  for(vector<string>::iterator it = existent_files.begin() ; ( it!=existent_files.end() && !revision_found ) ; ++it) {
    revision_file_id = (*it);
    if( !dh_.get_file_info(revision_file_id,revision_file_info,status) ){ return false; }
    if( revision_file_info.name.compare(name)==0 && revision_file_info.extension.compare(extension)==0 ){ revision_found = true; }
  }
  return true;
}


bool RequestDispatcher::increase_file_revision(string file_id, int& status){
  DataHandler::file_info_st file_info;
  if( !dh_.get_file_info(file_id,file_info,status) ){ return false; }
  
  size_t revision = stoul_decimal(file_info.revision) + 1;
  
  return( dh_.modify_file_revision(file_id,to_string(revision),status) );
}


bool RequestDispatcher::add_info_files_from_id_list(string file_id_list, int mode, string text,
                                                    vector<RequestDispatcher::info_element_st>& files_vector, int& status){
  vector<string> file_ids = split_string(file_id_list,LABEL_STRING_DELIMITER);
  for(vector<string>::iterator it = file_ids.begin() ; it!=file_ids.end() ; ++it) {
    string file_id = (*it);
    DataHandler::file_info_st file_info;
    RequestDispatcher::info_element_st info_element;
    if( !dh_.get_file_info(file_id,file_info,status) ){ return false; }
    
    if(mode==SEARCH_MODE_OFF){ fill_info_elem_to_file_info(file_id,file_info,info_element); files_vector.push_back(info_element);}
    if(mode==SEARCH_MODE_BY_NAME){
      if( is_str_included_to_lower(file_info.name,text) ){ 
        fill_info_elem_to_file_info(file_id,file_info,info_element);  
        files_vector.push_back(info_element);
      }
    }
    if(mode==SEARCH_MODE_BY_EXTENSION){
      if( is_str_included_to_lower(file_info.extension,text) ){ 
        fill_info_elem_to_file_info(file_id,file_info,info_element);  
        files_vector.push_back(info_element);
      }
    }
    if(mode==SEARCH_MODE_BY_TAG){
      if( is_str_included_to_lower(file_info.tags,text) ){ 
        fill_info_elem_to_file_info(file_id,file_info,info_element);  
        files_vector.push_back(info_element);
      }
    }
    if(mode==SEARCH_MODE_BY_USER){ 
      if( file_info.owner.compare(text)==0 ){ 
        fill_info_elem_to_file_info(file_id,file_info,info_element);  
        files_vector.push_back(info_element);
      }
    }
    
  } // End for()
  return true;  
}


bool RequestDispatcher::add_info_dirs_from_id_list(string dir_id_list,
                                                   vector< RequestDispatcher::info_element_st >& directories_vector, int& status){
  vector<string> dir_ids = split_string(dir_id_list,LABEL_STRING_DELIMITER);
  for(vector<string>::iterator it = dir_ids.begin() ; it!=dir_ids.end() ; ++it) {
    string dir_id = (*it);
    DataHandler::dir_info_st dir_info;
    RequestDispatcher::info_element_st info_element;
    if( !dh_.get_directory_info(dir_id,dir_info,status) ){ return false; }
    fill_info_elem_to_dir_info(dir_id,dir_info,info_element);
    directories_vector.push_back(info_element);
  }  
  return true;  
}


bool RequestDispatcher::recover_deleted_files_from_user_info(DataHandler::user_info_st user_info, int status){
  
// TODO(mart): Ver el caso donde borro un archivo y el usuario sube a la misma carpeta un archivo con el mismo nombre
// (posible conflicto de revisiones). Se podría resolver renombrando el viejo archivo recuperado (agregando el nombre "restored" o "(1)" )
  
  vector<string> files_deleted = split_string(user_info.files_deleted,LABEL_STRING_DELIMITER);
  for(vector<string>::iterator it = files_deleted.begin() ; it!=files_deleted.end() ; ++it) {
    string file_deleted_id = (*it);
    DataHandler::file_info_st file_info;
    if( !dh_.get_file_info(file_deleted_id,file_info,status) ){ return false; }
    string new_dir_id = file_info.parent_directory;
    DataHandler::dir_info_st dir_info;
    // If the old parent directory is deleted, assign the root directory
    if( !dh_.get_directory_info(file_info.parent_directory,dir_info,status) ){
      new_dir_id = user_info.dir_root;
      if( !dh_.get_directory_info(new_dir_id,dir_info,status) ){ return false; }
    }
    // Add file id to files_contained
    dir_info.files_contained = add_key_to_string_list(dir_info.files_contained,file_deleted_id);
    if( !dh_.modify_directory_files_contained(new_dir_id,dir_info.files_contained,status) ){ return false; }

    // Change size to directory container of the file
    if( !increase_dir_size_recursive(new_dir_id,file_info.size,status) ){ return false; }
    
  }
  return true;
}


void RequestDispatcher::divide_selected_ids(string original_ids, vector<string> selected_ids,
                                            string& coincidence_ids, string& no_coincidence_ids){
  vector<string> original_ids_v = split_string(original_ids,LABEL_STRING_DELIMITER);
  for(vector<string>::iterator it = original_ids_v.begin() ; it!=original_ids_v.end() ; ++it) {
    string original_id = (*it);
    bool id_found = false;
    for(vector<string>::iterator it2 = selected_ids.begin() ; ( it2!=selected_ids.end() && !id_found ) ; ++it2) {
      string id_selected = (*it2);
      if( original_id.compare(id_selected)==0 ){ coincidence_ids.append(LABEL_STRING_DELIMITER+original_id); id_found = true; }
    }
    if( !id_found ){ no_coincidence_ids.append(LABEL_STRING_DELIMITER+original_id); }
  }
  return void();
}


bool RequestDispatcher::purge_deleted_files_from_user_info(DataHandler::user_info_st user_info, int status){

  // Delete files
  vector<string> files_deleted = split_string(user_info.files_deleted,LABEL_STRING_DELIMITER);
  for(vector<string>::iterator it = files_deleted.begin() ; it!=files_deleted.end() ; ++it) {
    string file_id = (*it);
    DataHandler::file_info_st file_info;
    if( !dh_.get_file_info(file_id,file_info,status) ){ return false; }
    
    // Physical delete file (with all revisions)
    for (int i = stoi(file_info.revision) ; i > 0; --i) {
      string file_name = file_info.owner + file_id + to_string(i);
      fh_.delete_file(file_name);  
    }
    
    // Delete file information on DataBase
    if( !dh_.delete_file(file_id,status) ){ return false; }
    
    // Change size to user quota
    if( !decrease_user_quota_used(file_info.owner,file_info.size,status) ){ return false; }
  }
  
  return true;
}


bool RequestDispatcher::add_tags_recursive(string dir_id, vector<string>& tags, int& status){
 
  // Get directory info
  DataHandler::dir_info_st dir_info;
  if( !dh_.get_directory_info(dir_id,dir_info,status) ){ return false; }

  // Add tags from this directory
  vector<string> dir_tags = split_string(dir_info.tags,LABEL_STRING_DELIMITER);
  if( dir_tags.size()!=0 ){ tags.insert(tags.end(),dir_tags.begin(),dir_tags.end()); }
  
  // Add tags from files contained in this directory
  if( !add_tags_from_id_list(dir_info.files_contained,tags,status) ){ return false; }
  
  // Step into sub-directories recursively
  vector<string> subdir_ids = split_string(dir_info.directories_contained,LABEL_STRING_DELIMITER);
  for(vector<string>::iterator it = subdir_ids.begin() ; it!=subdir_ids.end() ; ++it) {
    string subdir_id = (*it);
    if( !add_tags_recursive(subdir_id,tags,status) ){ return false; }
  }
  
  return true;
}


bool RequestDispatcher::add_tags_from_id_list(string file_ids, vector<string>& tags, int& status){
  vector<string> file_ids_v = split_string(file_ids,LABEL_STRING_DELIMITER);  
  for(vector<string>::iterator it = file_ids_v.begin() ; it!=file_ids_v.end() ; ++it) {
    string file_id = (*it);
    DataHandler::file_info_st file_info;
    if( !dh_.get_file_info(file_id,file_info,status) ){ return false; }
    vector<string> file_tags = split_string(file_info.tags,LABEL_STRING_DELIMITER);
    if( file_tags.size()!=0 ){ tags.insert(tags.end(),file_tags.begin(),file_tags.end()); }
  }
  return true;
}


bool RequestDispatcher::search_by_mode_recursive(string dir_id, int mode, string text,
                                                      vector<RequestDispatcher::info_element_st>& elements_founded, int &status){
  // Get directory info
  DataHandler::dir_info_st dir_info;
  if( !dh_.get_directory_info(dir_id,dir_info,status) ){ return false; }

  // Check this dir (by mode)
  if(mode==SEARCH_MODE_BY_NAME){ 
    if( is_str_included_to_lower(dir_info.name,text) ){ 
      RequestDispatcher::info_element_st info_element_dir;
      fill_info_elem_to_dir_info(dir_id,dir_info,info_element_dir);  
      elements_founded.push_back(info_element_dir);
    }  
  }  
  if(mode==SEARCH_MODE_BY_TAG){ 
    if( is_str_included_to_lower(dir_info.tags,text) ){ 
      RequestDispatcher::info_element_st info_element_dir;
      fill_info_elem_to_dir_info(dir_id,dir_info,info_element_dir);  
      elements_founded.push_back(info_element_dir);
    }
  }
  
  // Check files contained in this dir
  if( !add_info_files_from_id_list(dir_info.files_contained,mode,text,elements_founded,status) ){ return false; }
  
  // Step into sub-directories recursively
  vector<string> subdir_ids = split_string(dir_info.directories_contained,LABEL_STRING_DELIMITER);
  for(vector<string>::iterator it = subdir_ids.begin() ; it!=subdir_ids.end() ; ++it) {
    string subdir_id = (*it);
    if( !search_by_mode_recursive(subdir_id,mode,text,elements_founded,status) ){ return false; }
  }
  
  return true;
}


void RequestDispatcher::fill_info_elem_to_dir_info(string dir_id, DataHandler::dir_info_st dir_info,
                                                        RequestDispatcher::info_element_st& info_element){
  info_element.id = stoul_decimal(dir_id);
  info_element.lastModDate = dir_info.date_last_mod;
  info_element.name = dir_info.name;
  info_element.type = LABEL_D;  
  info_element.shared = LABEL_FALSE; // Directory is always shared==false 
  info_element.size = stoul_decimal(dir_info.size);
  info_element.owner = dir_info.owner;

  // Calculate number of items
  vector<string> temp_subdir_ids = split_string(dir_info.directories_contained,LABEL_STRING_DELIMITER);
  vector<string> temp_file_ids = split_string(dir_info.files_contained,LABEL_STRING_DELIMITER);
  info_element.number_of_items = temp_subdir_ids.size() + temp_file_ids.size();
}


void RequestDispatcher::fill_info_elem_to_file_info(string file_id, 
                                           DataHandler::file_info_st file_info, RequestDispatcher::info_element_st& info_element){
  // Fill info
  info_element.id = stoul_decimal(file_id);
  info_element.lastModDate = file_info.date_last_mod;
  info_element.name = file_info.name;
  info_element.type = LABEL_A;  
  info_element.size = stoul_decimal(file_info.size);
  info_element.number_of_items = 0; // File is always number_of_items==0 
  info_element.owner = file_info.owner;
  
  // Calculate number of users shared
  vector<string> temp_users_shared = split_string(file_info.users_shared,LABEL_STRING_DELIMITER);
  size_t number_of_users_shared = temp_users_shared.size();
  if(number_of_users_shared > 0){ info_element.shared = LABEL_TRUE; }else{ info_element.shared = LABEL_FALSE; }

  return void();
}


bool RequestDispatcher::is_str_included_to_lower(string str_source, string str_to_search){
  transform(str_source.begin(),str_source.end(),str_source.begin(), ::tolower);
  transform(str_to_search.begin(),str_to_search.end(),str_to_search.begin(), ::tolower);
  return( str_source.find(str_to_search)!=string::npos );
}


void RequestDispatcher::fill_user_info_st(string user_id, DataHandler::user_info_st dh_user_info, RequestDispatcher::user_info_st& user_info){

  user_info.id = user_id;
  user_info.email = dh_user_info.email;
  vector<string> name = split_string(dh_user_info.name,LABEL_STRING_DELIMITER);
  user_info.first_name = name.front();
  user_info.last_name = name.back();
  vector<string> location = split_string(dh_user_info.location,LABEL_STRING_DELIMITER);
  user_info.gps_lat = location.front();
  user_info.gps_lon = location.back();
  user_info.user_quota_used = dh_user_info.user_quota_used;
  user_info.user_quota_total = to_string(max_user_quota_);
  float percentage = ( (float)stoul_decimal(dh_user_info.user_quota_used) * (float)100) / (float)max_user_quota_;
  stringstream stream;
  stream << std::fixed << std::setprecision(2) << percentage;
  user_info.user_quota_used_percentage = stream.str() + "%";

  return void();
}


