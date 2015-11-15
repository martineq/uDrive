#ifndef REQUESTDISPATCHER_H
#define REQUESTDISPATCHER_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>


#include "data_handler.h"
#include "file_handler.h"
#include "zip_handler.h"
#include "util/log.h"
#include "util/config_parser.h"


/**
 * @brief Request Dispatcher
 * 
 */
class RequestDispatcher{

  public:

    struct info_element_st {
      size_t id;
      string name;
      size_t size;
      string type;
      size_t number_of_items;
      string shared;
      string lastModDate;
      string owner;
    } ;

    struct user_info_st {
      string email;
      string first_name;
      string last_name;
      string gps_lat;
      string gps_lon;
      string user_quota_used;
      string user_quota_used_percentage;
      string user_quota_total;
    } ;
    
    struct dir_info_st {
      string name;
      string date_last_mod;
      string tags;
      string owner;
      string parent_directory;
      string size;
      vector<RequestDispatcher::info_element_st> directory_element_info;
    } ;
    
    struct file_info_st {
      string name;
      string extension;
      string date_last_mod;
      string user_last_mod;
      string tags;
      string size;
      string revision;
    } ;
  
  
  private:
    
    DataHandler dh_;
    FileHandler fh_;
    ZipHandler zh_;
    size_t max_user_quota_;
    static RequestDispatcher* request_dispatcher_instance_;
    bool init_ok_ = false;
    
    RequestDispatcher(string database_path,size_t max_user_quota);
    bool init(string database_path,size_t max_user_quota);
    bool get_user_quota_used(string user_id, string& quota, int& status);
    bool increase_user_quota_used(string user_id, string quota_increased, int& status);
    bool decrease_user_quota_used(string user_id, string quota_decreased, int& status); 
    bool change_dir_date_recursive(string dir_id, string new_date, int& status);
    bool increase_dir_size_recursive(string dir_id, string size_increased, int& status);
    bool decrease_dir_size_recursive(string dir_id, string size_decreased, int& status);
    bool get_root_dir_id(string user_id, string& root_dir_id, int& status);
    vector<string> split_string(string string_to_split, char delimiter);
    bool db_is_initiated();
    ZipHandler::dir_tree_node_st get_dir_structure_recursive(string dir_id, int& status);
    unsigned long stoul_decimal(const string& str);
    string add_key_to_string_list(string list, string key);
    string remove_key_from_string_list(string list, string key);
    bool get_directory_element_info_from_dir_info(DataHandler::dir_info_st dir_info,vector<RequestDispatcher::info_element_st>& directory_element_info, int& status);
    bool delete_dir_recursive(string dir_id, int& status);
    bool search_revision_file(string parent_dir_id, string name, string extension, bool &revision_found, string &revision_file_id, DataHandler::file_info_st &revision_file_info, int status);
    bool increase_file_revision(string file_id, int& status);
    bool add_info_files_from_id_list(string file_id_list, vector<RequestDispatcher::info_element_st>& files_vector, int& status);    
    bool add_info_dirs_from_id_list(string dir_id_list, vector<RequestDispatcher::info_element_st>& directories_vector, int& status);    
    bool recover_deleted_files_from_user_info(DataHandler::user_info_st user_info,int status);
    void divide_selected_ids(string original_ids, vector<string> selected_ids, string &coincidence_ids, string &no_coincidence_ids);
    bool purge_deleted_files_from_user_info(DataHandler::user_info_st user_info,int status);
    bool add_tags_recursive(string dir_id, vector<string> &tags, int& status);
    bool add_tags_from_id_list(string file_ids, vector<string>& tags, int& status);
    
  public:  
    
    ~RequestDispatcher();   
    
    /**
     * @brief Returns the instance of the class. Returs nullptr if the DB is not initiated previously
     * 
     * @return RequestDispatcher*
     */
    static RequestDispatcher *get_instance() {
        if(request_dispatcher_instance_==nullptr){ 
          ConfigParser::Configuration config;
          if ( (ConfigParser::takeConfFromFile(config))!=0 ){ return nullptr;} 
          request_dispatcher_instance_= new RequestDispatcher(config.dbpath,config.maxquotauser); 
        }
        if( !(request_dispatcher_instance_->db_is_initiated()) ){ return nullptr;} 
        return request_dispatcher_instance_;
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
    * @param new_token 
    * @param date 
    * @param user_id returns user ID
    * @param status returns DataHandler status ONLY if @return==false
    * @return bool
    */
    bool sign_up(string email, string password, string first_name, string last_name, string gps_lat, string gps_lon, string new_token, string date, string& user_id, int& status);
    
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
    bool log_in(string email, string password, string new_token, string& user_id, int& status);
    
    
    /**
     * @brief Verifies if the user_token correspond to the user_id. 
     *        Returns true if the token is valid, false in other case.
     * 
     * @param user_id ...
     * @param user_token ...
     * @param status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool check_token(string user_id, string user_token, int& status);
    
    
    /**
     * @brief Creates a new directory. Returns true on success.
     *        On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param user_id ...
     * @param name ...
     * @param date ...
     * @param parent_dir_id ...
     * @param dir_id Returns directory ID
     * @param status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool new_directory(string user_id, string name, string date, string parent_dir_id, string& dir_id, int& status);
    
    /**
     * @brief Saves a new file. Returns true on success.
     *        On error returns false and a DataHandler status (see db_constants.h)
     *        
     * @param user_id ...
     * @param name ...
     * @param extension ...
     * @param date ...
     * @param p_file_stream ...
     * @param size ...
     * @param parent_dir_id ...
     * @param file_id ...
     * @param status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool new_file(string user_id, string name, string extension, string date, const char* p_file_stream, string size, string parent_dir_id, string& file_id, int& status);
    
    /**
     * @brief Gets the user information on a DataHandler::user_info_st.  Returns true on success.
     *        On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param user_id ...
     * @param user_info returns DataHandler::user_info_st
     * @param status  returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool get_user_info(string user_id, RequestDispatcher::user_info_st& user_info, int& status);
    
    /**
     * @brief gets the image file for a user.  Returns true on success.
     *        On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param user_id ...
     * @param p_image_stream return the image stream. The user must de-allocate the file.
     * @param size return the size of the stream (in bytes)
     * @param status ...
     * @return bool
     */
    bool get_user_image(string user_id, char*& p_image_stream, string& size, int& status);
    
    /**
     * @brief Gets the directory information on a DataHandler::dir_info_st (only non-deleted files)
     *        Returns true on success.
     *        On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param user_id ...
     * @param dir_id ...
     * @param dir_info returns DataHandler::dir_info_st
     * @param status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool get_directory_info(string user_id, string dir_id, RequestDispatcher::dir_info_st& dir_info, int& status);
    
    /**
     * @brief Gets the file information on a  DataHandler::file_info_st.  Returns true on success.
     *        On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param user_id ...
     * @param file_id ...
     * @param file_info returns DataHandler::file_info_st
     * @param status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool get_file_info(string user_id, string file_id, RequestDispatcher::file_info_st& file_info, int& status);
    
    /**
     * @brief Gets the file stream for a file_id. Returns true on success.
     *        On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param user_id ...
     * @param file_id ...
     * @param revision ...
     * @param p_file_stream return the file stream. The user must de-allocate the file.
     * @param size_stream return the size of the stream (in bytes)
     * @param status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool get_file_stream(string user_id, string file_id, string revision, char*& p_file_stream, string& size_stream, int& status);
    
    /**
     * @brief Gets the stream for a dir_id (in a zip file). Returns true on success.
     *        On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param user_id ...
     * @param dir_id ...
     * @param p_dir_stream return the dir stream. The user must de-allocate the file.
     * @param size_stream return the size of the stream (in bytes)
     * @param status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool get_dir_stream(string user_id, string dir_id, char*& p_dir_stream, string& size_stream, int& status);
    
    
    /**
     * @brief Gets a vector of elements (files) contained in shared files for an user. Returns true on success.
     *        On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param user_id ...
     * @param shared_files return vector of RequestDispatcher::info_element_st
     * @param status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool get_shared_files(string user_id,vector<RequestDispatcher::info_element_st>& shared_files, int& status);
    
   
    /**
     * @brief Gets a vector of elements (files) contained in deleted files for an user (recycle bin). 
     *        Returns true on success. On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param user_id ...
     * @param deleted_files return vector of RequestDispatcher::info_element_st
     * @param status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool get_deleted_files(string user_id,vector<RequestDispatcher::info_element_st>& deleted_files, int& status);
    
    
    /**
     * @brief Gets a vector of tags used by the user in their files and tags used in shared files. 
     *        Returns true on success. On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param user_id ...
     * @param tags ...
     * @param status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool get_tags(string user_id, vector<string> &tags, int& status);
    
    /**
     * @brief Gets the full list of email for all registered users
     *        Returns true on success. On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param pattern ...
     * @param status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool get_user_email_list_full(vector<string> &list, int& status);
    
    /**
     * @brief Gets the list of email for all registered users, filtered by pattern
     *        Returns true on success. On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param pattern ...
     * @param status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool get_user_email_list_by_pattern(string pattern, vector<string> &list, int& status);
    
    /**
     * @brief Sets the image file for an user.  Returns true on success.
     *        On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param user_id ...
     * @param p_image_stream ...
     * @param size ...
     * @param status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool set_user_image(string user_id, const char* p_image_stream, string size, int& status);
    
    /**
     * @brief Sets a new user to share a file. Returns true on success.
     *        On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param user_owner_id ...
     * @param file_id ...
     * @param user_shared_id ...
     * @param date ...
     * @param status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool set_file_share(string user_owner_id, string file_id, string user_shared_id, string date, int& status);
    
    
    /**
     * @brief Remove a user from file sharing. Returns true on success.
     *        On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param user_owner_id ...
     * @param file_id ...
     * @param user_shared_id ...
     * @param date ...
     * @param status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool unset_file_share(string user_owner_id, string file_id, string user_shared_id, string date, int& status);
    
        
    /**
     * @brief Modifies information of the user. Returns true on success.
     *        On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param user_id ...
     * @param email ...
     * @param password ...
     * @param name ...
     * @param location ...
     * @param status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool modify_user_info(string user_id, string email, string password, string name, string location, int& status);
    
    
    /**
     * @brief Modifies information of the directory. Returns true on success.
     *        On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param user_id ...
     * @param dir_id ...
     * @param name ...
     * @param date ...
     * @param tags ...
     * @param status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool modify_directory_info(string user_id, string dir_id, string name, string date, string tags, int& status);
    

    /**
     * @brief Modifies information of the file. Returns true on success.
     *        On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param user_id ...
     * @param file_id ...
     * @param name ...
     * @param extension ...
     * @param date ...
     * @param tags ...
     * @param status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool modify_file_info(string user_id, string file_id, string name, string extension, string date, string tags, int& status);

    
    /**
     * @brief Deletes all info linked to the user: deletes the root directory, sub-directories and 
     *        files contanined (logically and physically). Also removes the profile info of the user
     *        Returns true on success. On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param user_id ...
     * @param status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    
    bool delete_user(string user_id, int& status);
    

    /**
     * @brief Deletes the directory, sub-directories and files contanined (logically). 
     *        Puts file into a "user deleted files" sector, fo a future purge or recover.
     *        Deleted files must not be asked by the client.
     *        Returns true on success. On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param user_id ...
     * @param dir_id ...
     * @param status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool delete_directory(string user_id, string dir_id, int& status);
    
    
    /**
     * @brief Deletes reference of the file from their parent directory (the file will not be informed to the user). 
     *        Also puts file into a "user deleted files" sector, fo a future purge or recover.
     *        Deleted files must not be asked by the client.
     *        Returns true on success. On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param user_id ...
     * @param file_id ...
     * @param status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool delete_file(string user_id, string file_id, int& status);  

    
    /**
     * @brief Deletes physically all files (and their revisions) previously deleted
     *        Returns true on success. On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param user_id ...
     * @param status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool purge_deleted_files(string user_id, int& status);
    
    
    /**
     * @brief Deletes physically all files (and their revisions) previously deleted
     *        Returns true on success. On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param user_id ...
     * @param selected_files_id ...
     * @param status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool purge_deleted_files(string user_id, vector<string> selected_files_id, int& status);

    
    /**
     * @brief Recover files deleted, making files visible. If the original parent dir not exists, move the file to the root dir.
     *        Returns true on success. On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param user_id ...
     * @param status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool recover_deleted_files(string user_id, int& status);
    
    
    /**
     * @brief Recover files deleted, making files visible. If the original parent dir not exists, move the file to the root dir.
     *        Returns true on success. On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param user_id ...
     * @param selected_files_id ...
     * @param status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool recover_deleted_files(string user_id, vector<string> selected_files_id, int& status);
       
    
    
    bool HARDCODED_get_user_image(string user_id, string& image_stream, int& status);

};

#endif // REQUESTDISPATCHER_H

