#ifndef ZIPHANDLER_H
#define ZIPHANDLER_H

#include <string>
#include <vector>

using namespace std;

class ZipHandler{
  
  public:

    struct dir_tree_node_st {
      string type;    
      size_t id;
      string name;
      vector<ZipHandler::dir_tree_node_st> sub_elem;
    } ;

    ZipHandler();
    ~ZipHandler();

    // directory_structure must contain directory_structure
    bool create_zip(string zip_name,ZipHandler::dir_tree_node_st dir_structure);
    
  private:
    
    bool create_file_structure_recursive(bool is_root, ZipHandler::dir_tree_node_st dir_structure);
  
};

#endif // ZIPHANDLER_H
