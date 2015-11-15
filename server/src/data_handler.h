#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H

#include <iostream>
#include <string>
#include <vector>
#include "db_handler.h"
#include "db_constants.h"

using namespace std;

class DataHandler {

  private:
    DbHandler dbh_;
    
    /**
    * @brief Returns the key for a query of an user item
    * 
    * @param email email of the user
    * @param item_selected item selected of the user for query
    * @return std::string
    */
    string generate_user_key(string user_id, string item_selected);
    
    /**
    * @brief Returns the key for a query of an directory item
    * 
    * @param dir_id ID of the selected directory
    * @param item_selected item selected of the user for query
    * @return std::string
    */
    string generate_dir_key(string dir_id, string item_selected);
    
    /**
    * @brief Returns the key for a query of an file item
    * 
    * @param file_id ID of the selected file
    * @param item_selected item selected of the user for query
    * @return std::string
    */
    string generate_file_key(string file_id, string item_selected);
    
    /**
    * @brief Returns the key for a query of an ticket type
    * 
    * @param ticket_type type of ticket selected
    * @return std::string
    */
    string generate_ticket_key(string ticket_type);
    
    /**
    * @brief Obtains a new id for type_of_id. Returns true on success.
    * 
    * @param type_of_id the type of the requested id
    * @param id the new ID generated
    * @return bool
    */
    bool create_id(string type_of_id,string& id);
    
    /**
    * @brief Adds an user_id for their corresponding email in a index. Returns true if the operation was successful
    * 
    * @param email ...
    * @param user_id ...
    * @return bool
    */
    bool add_email_user_id_index(string email, string user_id);
    
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
    bool get_email_user_id_index(string email, string& user_id, int& status);
    
    /**
    * @brief Verifies the type_of_id ticket number (and creates if they not exist).
    * The ticket number is used in the assignment of ID's
    * 
    * @return void
    */
    void init_id_ticket(string type_of_id);
    
    
    /**
     * @brief Initiates the list of users
     * 
     * @return void
     */
    void init_user_list();
    
    /**
    * @brief Prints a message error on std::cerr
    * 
    * @return void
    */
    void print_db_error();
    
    /**
    * @brief Wrapper for DbHandler::get(std::string key, std::string* value, bool& found). (used on member variable dbh_). 
    *        Includes the status for a get operation.
    * 
    * @param key ...
    * @param value ...
    * @param status ...
    * @return bool
    */
    bool dbh_get(string key, string* value, int& status);
    
    /**
    * @brief Wrapper for DbHandler::put(std::string key, std::string value). (used on member variable dbh_). 
    *        Includes the status for a put operation.
    * 
    * @param key ...
    * @param value ...
    * @param status ...
    * @return bool
    */
    bool dbh_put(string key, string value, int& status);

    /**
     * @brief Add user email to email list
     * 
     * @param email ...
     * @param status ...
     * @return bool
     */
    bool add_email_to_list(string email, int status);

    /**
     * @brief Remove user email from email list
     * 
     * @param email ...
     * @param status ...
     * @return bool
     */
    bool remove_email_from_list(string email, int status);
    
    /**
     * @brief Splits a string with a delimiter in string tokens 
     * 
     * @param string_to_split ...
     * @param delimiter ...
     * @return std::vector< std::string >
     */
    vector<string> split_string(string string_to_split, char delimiter);

    
  public:

    struct user_info_st {
      string email;
      string name;
      string location;
      string dir_root;
      string shared_files;
      string user_quota_used;
      string files_deleted;
    } ;

    struct dir_info_st {
      string name;
      string date_last_mod;
      string tags;
      string owner;
      string parent_directory;
      string files_contained;
      string directories_contained;
      string size;
    } ;
    
    struct file_info_st {
      string name;
      string extension;
      string date_last_mod;
      string user_last_mod;
      string tags;
      string owner;
      string size;
      string users_shared;
      string revision;
      string parent_directory;
    } ;
    
    DataHandler();
    ~DataHandler();

    /**
    * @brief Initiates the database. 
    * Returns true if the DB is successfully open. On error returns false.
    * 
    * @param  ...
    * @return bool
    */
    bool init(string database_path);
    
    /**
    * @brief Adds a new user on the DB. (Used in sign up)
    *        Creates a "root" directory and user ID. Returns true on success.
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
    bool add_user(string email, string password, string name, string location, string token, string date, string& user_id, int& status);
    
    /**
    * @brief Adds a new token for a existent user (Used in logins AKA sing in). Returns the user_id on success.
    *        Returns true on success. On error returns false and a DataHandler status (see db_constants.h)
    *        (Used in combination get_user_password() + add_user_token() for user login)
    * 
    * @param email ...
    * @param token ...
    * @param user_id returns user ID
    * @param status returns DataHandler status ONLY if @return==false
    * @return bool
    */
    bool add_user_token(string email, string token, string& user_id, int& status);
    
    /**
    * @brief Adds a new directory for an user_id. ("new directory")
    *        Creates dir ID and links to their parent dir. Returns true on success.
    *        On error returns false and a DataHandler status (see db_constants.h)
    * 
    * @param user_id ...
    * @param name ...
    * @param date ...
    * @param parent_dir_id add "LABEL_NO_PARENT_DIR" as ID if the directory is the root directory.
    * @param dir_id returns directory ID
    * @param status returns DataHandler status ONLY if @return==false
    * @return bool
    */
    bool add_directory(string user_id, string name, string date, string parent_dir_id, string& dir_id, int& status);
    
    /**
    * @brief Adds a new file for an user_id and contained to a dir_id. ("new file")
    *        Creates file ID and links to their parent dir. Returns true on success.
    *        On error returns false and a DataHandler status (see db_constants.h).
    * 
    * @param user_id ...
    * @param name ...
    * @param extension ...
    * @param date ...
    * @param size ...
    * @param revision revision of the file
    * @param dir_id container directory of the file
    * @param file_id returns the file_id
    * @param status returns DataHandler status ONLY if @return==false
    * @return bool
    */
    bool add_file(string user_id, string name, string extension, string date, string size, string revision, string parent_dir_id, string& file_id, int& status);

    
    
    /**
    * @brief Obtains the password from an user email. (Used in logins AKA sing in, for checking password account)
    *        Returns true on success.
    *        On error returns false and a DataHandler status (see db_constants.h)
    *        (Used in combination get_user_password() + add_user_token() for user login)
    * 
    * @param email ...
    * @param password returns the user passcode
    * @param status returns DataHandler status ONLY if @return==false
    * @return bool
    */
    bool get_user_password(string email, string& password, int& status);
    
    /**
    * @brief Obtains the token from an user_id.
    *        Returns true on success.
    *        On error returns false and a DataHandler status (see db_constants.h)
    *        (Used in token authentication, comparing with client token)
    * 
    * @param user_id ...
    * @param token returns the token for the user
    * @param status returns DataHandler status ONLY if @return==false
    * @return bool
    */
    bool get_user_token(string user_id, string& token, int& status);
    
    /**
    * @brief Obtains user information in a user_info_st struct.
    *        Returns true on success.
    *        On error returns false and a DataHandler status (see db_constants.h)
    *        ( To get user password/token use get_user_password()/get_user_token() )
    * 
    * @param user_id ...
    * @param user_info On success retuns user information
    * @param status returns DataHandler status ONLY if @return==false
    * @return bool
    */
    bool get_user_info(string user_id, user_info_st& user_info, int& status);
    
    /**
    * @brief Obtains directory information in a dir_info_st struct.
    *        Returns true on success.
    *        On error returns false and a DataHandler status (see db_constants.h)
    * 
    * @param dir_id ...
    * @param directory_info On success retuns directory information
    * @param status returns DataHandler status ONLY if @return==false
    * @return bool
    */
    bool get_directory_info(string dir_id, dir_info_st& dir_info, int& status);
    
    /**
    * @brief Obtains file information in a file_info_st struct.
    *        Returns true on success.
    *        On error returns false and a DataHandler status (see db_constants.h)
    * 
    * @param file_id ...
    * @param file_info On success retuns file information
    * @param status returns DataHandler status ONLY if @return==false
    * @return bool
    */
    bool get_file_info(string file_id, file_info_st& file_info, int& status);
    
    
    /**
     * @brief Gets the list of email for all registered users
     * 
     * @param email_list ...
     * @return bool
     */
    bool get_user_mail_list(string& email_list, int &status);
    
    /**
    * @brief Deletes all information for the user ID and their Index entry. Returns true on success.
    *        On error returns false and a DataHandler status (see db_constants.h)
    *        (Warning: all info will be deleted without any check for files or directories)
    * 
    * @param user_id ...
    * @param status returns DataHandler status ONLY if @return==false
    * @return bool
    */
    bool delete_user(string user_id, int& status);
    
    /**
    * @brief Deletes all information for the dir ID. Returns true on success.
    *        On error returns false and a DataHandler status (see db_constants.h)
    *        (Warning: all info will be deleted without any check for files or subdirectories)
    * 
    * @param dir_id ...
    * @param status returns DataHandler status ONLY if @return==false
    * @return bool
    */
    bool delete_directory(string dir_id, int& status);
    
    /**
    * @brief Deletes all information for the file ID. Returns true on success.
    *        On error returns false and a DataHandler status (see db_constants.h)
    * 
    * @param file_id ...
    * @param status returns DataHandler status ONLY if @return==false
    * @return bool
    */
    bool delete_file(string file_id, int& status);
    
    
    /**
    * @brief Modifies user password for an user_id. Returns true on success.
    *        On error returns false and a DataHandler status (see db_constants.h)
    * 
    * @param user_id ...
    * @param password ...
    * @param status status returns DataHandler status ONLY if @return==false
    * @return bool
    */
    bool modify_user_password(string user_id, string password, int& status);
    
    /**
    * @brief Modifies user information for an user_id. Returns true on success.
    *        On error returns false and a DataHandler status (see db_constants.h)
    * 
    * @param user_id ...
    * @param email ...
    * @param name ...
    * @param location ...
    * @param files_shared ID of the files shared (from other owners)
    * @param status status returns DataHandler status ONLY if @return==false
    * @return bool
    */
    bool modify_user_info(string user_id, string email, string name, string location, string files_shared, string quota, string files_deleted, int& status);
    
    /**
    * @brief Modifies directory information for an dir_id. Returns true on success.
    *        On error returns false and a DataHandler status (see db_constants.h)
    * 
    * @param dir_id ...
    * @param name ...
    * @param date ...
    * @param tags ...
    * @param size ...
    * @param status status returns DataHandler status ONLY if @return==false
    * @return bool
    */
    bool modify_directory_info(string dir_id, string name, string date, string tags, string size, int& status);
    
    /**
     * @brief Modifies the list of files contained. Returns true on success.
     *        On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param dir_id ...
     * @param files_contained ...
     * @param status status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool modify_directory_files_contained(string dir_id, string files_contained, int& status);
    
    
    /**
     * @brief Modifies the list of directories contained. Returns true on success.
     *        On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param dir_id ...
     * @param directories_contained ...
     * @param status status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool modify_directory_dirs_contained(string dir_id, string directories_contained, int& status);
    
    /**
    * @brief Modifies file information for an file_id. Returns true on success.
    *        On error returns false and a DataHandler status (see db_constants.h)
    * 
    * @param file_id ...
    * @param name ...
    * @param extension ...
    * @param date ...
    * @param tags ...
    * @param users_shared user_id's of user that have shared de file
    * @param user_id_modifier ID of the user that modifies the info
    * @param users_deleted
    * @param status status returns DataHandler status ONLY if @return==false
    * @return bool
    */
    bool modify_file_info(string file_id, string name, string extension, string date, string tags, string users_shared, string user_id_modifier, string parent_dir, int& status);
    
    /**
    * @brief Modifies file revision for an file_id. Returns true on success.
    *        On error returns false and a DataHandler status (see db_constants.h)
    *     
    * @param file_id ...
    * @param revision ...
    * @param status status returns DataHandler status ONLY if @return==false
    * @return bool
    */
    bool modify_file_revision(string file_id, string revision, int& status);

};

#endif // DATA_HANDLER_H
