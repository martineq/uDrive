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
    
    string generate_user_key(string user_id, string item_selected);
    string generate_dir_key(string dir_id, string item_selected);
    string generate_file_key(string file_id, string item_selected);
    bool user_exists(string user_id);
    bool create_id(string type_of_id,string& id);
    bool add_email_to_index(string email, string user_id);
    bool get_user_id_from_email(string email, string& user_id);

  public:
    
    struct user_info_st {
      string email;
      string id;
      string password;
      string token;
      string name;
      string location;
      string dir_root;
      string shared_files;
    } ;

    struct dir_info_st {
      string id;
      string name;
      string date_last_mod;
      string user_last_mod;
      string tags;
      string owner;
      string parent_directory;
      string files_contained;
      string directories_contained;
    } ;
    
    struct file_info_st {
      string id;
      string name;
      string extension;
      string date_last_mod;
      string user_last_mod;
      string tags;
      string owner;
      string size;
      string deleted_status;
      string revision;
      string parent_revision;
      string users_shared;
    } ;

    
    DataHandler();
    ~DataHandler();

    bool init();
    
    // TODO (mart): change the type used on parameters "yyy_file_stream" for the selected type
    
    bool add_user(string email, string password, string name, string location, string token, string date, string& user_id , int& status);
    bool add_user_token(string email, string token, string& user_id, int& status);
    bool add_user_avatar(string user_id, string avatar_file_stream, int& status);
    bool add_directory(string user_id, string name, string date, string parent_dir_id, string& dir_id, int& status);
    bool add_file(string user_id, string name, string extension, string date, string directory, string& file_id, int& status);

    bool get_user_password(string email, string& password, int& status);
    bool get_user_token(string user_id, string& token, int& status);
    bool get_user_info(string user_id, user_info_st& user_info, int& status);
    bool get_user_avatar(string user_id, string& avatar_file_stream, int& status);
    bool get_directory_info(string user_id, dir_info_st& directory_info, int& status);
    bool get_file_info(string user_id, file_info_st& file_info, int& status);
    bool get_file_stream(string user_id, string& file_stream, int& status);
    
    bool delete_user(string user_id, int& status);
    bool delete_directory(string dir_id, int& status);
    bool delete_file(string file_id, int& status);
    
    bool modify_user_info(string user_id, string password, string name, string location, int& status);
    bool modify_directory_info(string dir_id, string name, string date, string tags, int& status);
    bool modify_file_info(string file_id, string name, string extension, string date, string tags, int& status);
    bool share_file(string file_id, string user_id_shared, int& status);
    bool unshare_file(string file_id, string user_id_unshared, int& status);
    
};

#endif // DATA_HANDLER_H
