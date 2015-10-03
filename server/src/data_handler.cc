#include "data_handler.h"

DataHandler::DataHandler(void){

}


DataHandler::~DataHandler(void){

}


bool DataHandler::add_user(string email, string password, string name, string location, string& token, int& status){
 
  return true;  
}


bool DataHandler::add_user_avatar(string email, string token, string file_stream, int& status){

  return true;  
}


bool DataHandler::add_directory(string email, string token, string name, string date, string parent_dir, string& dir_id, int& status){

  return true;  
}


bool DataHandler::add_file(string email, string token, string name, string extension, string date, string directory, string& file_id, int& status){

  return true;  
}


bool DataHandler::get_user_token(string email, string password, string& token, int& status){

  return true;  
}


bool DataHandler::get_user_info(string email, string token, string& user_info, int& status){

  return true;  
}


bool DataHandler::get_user_avatar(string email, string token, string& avatar_stream, int& status){

  return true;  
}


bool DataHandler::get_directory(string email, string token, string& directory_info, int& status){

  return true;  
}


bool DataHandler::get_shared_directory(string email, string token, string& shared_directory_info, int& status){

  return true;  
}


bool DataHandler::get_file(string email, string token, string& file_stream, int& status){

  return true;  
}


bool DataHandler::delete_user(string email, string token, int& status){

  return true;  
}


bool DataHandler::delete_directory(string email, string token, int& status){

  return true;  
}


bool DataHandler::delete_file(string email, string token, int& status){

  return true;  
}


bool DataHandler::modify_user_info(string email, string token, string password, string name, string location, int& status){

  return true;  
}


bool DataHandler::modify_directory(string email, string token, string name, string date, string tags, int& status){

  return true;  
}


bool DataHandler::modify_file_info(string email, string token, string name, string extension, string date, string user, int& status){

  return true;  
}


bool DataHandler::share_file(string email_owner, string token, string email_shared, int& status){

  return true;  
}


bool DataHandler::unshare_file(string email_owner, string token, string email_shared, int& status){

  return true;  
}

