#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H

#include <iostream>
#include <string>
#include "db_handler.h"
#include "db_constants.h"

using namespace std;

class DataHandler {

  private:
    DbHandler dbh_;
    
    string generate_user_key(string email, string item_selected);
    string generate_dir_key(string dir_id, string item_selected);
    string generate_file_key(string file_id, string item_selected);
    bool user_exists(string email);
    
    // TODO (mart): long get_quota(email)
  
  public:
    DataHandler();
    ~DataHandler();

    bool init();
    
    bool add_user(string email, string password, string name, string location, string token, string date, int& status);
    bool add_user_avatar(string email, string token, string file_stream, int& status);
    bool add_directory(string email, string token, string name, string date, string parent_dir, string& dir_id, int& status);
    bool add_file(string email, string token, string name, string extension, string date, string directory, string& file_id, int& status);

    bool get_user_token(string email, string password, string& token, int& status);
    bool get_user_info(string email, string token, string& user_info, int& status);
    bool get_user_avatar(string email, string token, string& avatar_stream, int& status);
    bool get_directory(string email, string token, string& directory_info, int& status);
    bool get_shared_directory(string email, string token, string& shared_directory_info, int& status);
    bool get_file(string email, string token, string& file_stream, int& status);

    bool delete_user(string email, string token, int& status);
    bool delete_directory(string email, string token, int& status);
    bool delete_file(string email, string token, int& status);

    bool modify_user_info(string email, string token, string password, string name, string location, int& status);
    bool modify_directory(string email, string token, string name, string date, string tags, int& status);
    bool modify_file_info(string email, string token, string name, string extension, string date, string user, int& status);
    bool share_file(string email_owner, string token, string email_shared, int& status);
    bool unshare_file(string email_owner, string token, string email_shared, int& status);

};

#endif // DATA_HANDLER_H
