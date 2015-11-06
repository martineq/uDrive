#ifndef ZIPHANDLER_H
#define ZIPHANDLER_H

#include <string>
#include <vector>

using namespace std;

class ZipHandler{

  public:

    struct file_st {
      string file_id;
      string file_real_name;
    } ;

    struct dir_tree_node_st {
      string dir_node_name;
      vector<ZipHandler::file_st> files_contained;
      vector<ZipHandler::dir_tree_node_st> sub_dirs;
    } ;

    ZipHandler();
    ~ZipHandler();

    /**
     * @brief Cretes a zip file in the root directory of the server with "zip_name.zip", base on the dir_structure.
     * 
     * 
     * @param zip_name ...
     * @param dir_structure ...
     * @return void
     */
    void create_zip(string zip_name, ZipHandler::dir_tree_node_st dir_structure);
    
  private:
    
    bool create_file_structure_recursive(string dir_path, ZipHandler::dir_tree_node_st dir_structure);
  
};

#endif // ZIPHANDLER_H
