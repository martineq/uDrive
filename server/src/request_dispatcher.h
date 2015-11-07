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


class RequestDispatcher{

  private:
    
    DataHandler dh_;
    FileHandler fh_;
    ZipHandler zh_;
    size_t max_user_quota_;
    static RequestDispatcher* request_dispatcher_instance;
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
    ZipHandler::dir_tree_node_st get_dir_structure_recursive(string user_id, string dir_id, int& status);
    unsigned long stoul_decimal(const string& str);
    
  public:

    struct info_element_st {
      size_t id;
      string name;
      size_t size;
      string type;
      size_t number_of_items;
      string shared;
      string lastModDate;
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
      string files_contained;
      string directories_contained;
      string size;
    } ;
    
    struct file_info_st {
      // TODO(mart): check if all of this variables are necesary
      string name;
      string extension;
      string date_last_mod;
      string user_last_mod;
      string tags;
      string owner;
      string size;
      string deleted_status;
      string users_shared;
      string revision;
    } ;
    
    ~RequestDispatcher();   
    
    /**
     * @brief Returns the instance of the class. Returs nullptr if the DB is not initiated previously
     * 
     * @return RequestDispatcher*
     */
    static RequestDispatcher *get_instance(string database_path,size_t max_user_quota){
        if(request_dispatcher_instance==nullptr){ request_dispatcher_instance= new RequestDispatcher(database_path,max_user_quota); }
        if( !(request_dispatcher_instance->db_is_initiated()) ){ return nullptr;} 
        return request_dispatcher_instance;
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
     *        TODO(mart): implement function: "If the Name and Dir ID is the same, check the hash with previous 
     *                                         revision and, if they are different, make new revision."
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
     * @brief Gets the directory information on a  DataHandler::dir_info_st.  Returns true on success.
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
     * @brief Sets the image file for an user.  Returns true on success.
     *        On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param user_id ...
     * @param p_image_stream ...
     * @param size ...
     * @param status ...
     * @return bool
     */
    bool set_user_image(string user_id, const char* p_image_stream, string size, int& status);
    
    
//     bool delete_user(string user_id, int& status);
//     bool delete_directory(string user_id, string dir_id, int& status);
//     bool delete_file(string user_id, string file_id, int& status);

//  bool modify_user_info(string user_id, string email, string password, string name, string location, string files_shared, int& status);
//  bool modify_directory_info(string dir_id, string name, string date, string tags, int& status);
//  bool modify_file_info(string file_id, string name, string extension, string date, string tags, string users_shared, string user_id_modifier, int& status);

    /**
     * @brief Gets a vector of elements (files or subdirectories) contained in a directory DataHandler::dir_info_st. Returns true on success.
     *        On error returns false and a DataHandler status (see db_constants.h)
     * 
     * @param user_id ...
     * @param dir_info ...
     * @param directory_element_info return vector of RequestDispatcher::info_element_st
     * @param status returns DataHandler status ONLY if @return==false
     * @return bool
     */
    bool get_directory_element_info_from_dir_info(string user_id, RequestDispatcher::dir_info_st dir_info,vector< RequestDispatcher::info_element_st >& directory_element_info, int& status);

};

#endif // REQUESTDISPATCHER_H

/*

Clase Request Dispatcher

TODO(mart): ¿Cómo se van a manejar las revisiones? Ver si hay que agregar parent_revision en cada file.
Casos de uso y funciones de Data Handler relacionadas: 
+ Modificar info usr        -> modify_user_info().            Verificar que el usr sea dueño.
+ Modificar info dir        -> modify_directory_info().       Verificar que el usr sea dueño.
+ Modificar info arch       -> modify_file_info().            En caso de ser dueño, verificar si el archivo está compartido y quitar ese estado a los demás usuarios. En caso de ser compartido sólo quitarse de la lista de compartidos.

*/