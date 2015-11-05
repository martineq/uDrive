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

    void create_zip(string zip_name, ZipHandler::dir_tree_node_st dir_structure);
    
    
  private:
    
    bool create_file_structure_recursive(string dir_path, ZipHandler::dir_tree_node_st dir_structure);
  
};

#endif // ZIPHANDLER_H
