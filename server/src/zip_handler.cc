#include "zip_handler.h"

ZipHandler::ZipHandler(){

}


ZipHandler::~ZipHandler(){

}


void ZipHandler::create_zip(string zip_name, ZipHandler::dir_tree_node_st dir_structure){

  // Create temporary directory structure
  create_file_structure_recursive("",dir_structure);
  
  // Create zip file
  // Example: "zip -r -1 directory.zip hola"). "-r": Recursive  "-1": Fast compression
  string command_zip ="zip -r -1 " + zip_name + " " + dir_structure.dir_node_name; 
  system(command_zip.c_str());
  
  // Delete temporary directory structure
  string remove_dir = "rm -rf " + dir_structure.dir_node_name;  // Example: "rm -rf hola"
  system(remove_dir.c_str());
  
  return void();
}


bool ZipHandler::create_file_structure_recursive(string dir_path, ZipHandler::dir_tree_node_st dir_structure){

  // Add new directory name to dir path and create directory
  if( !dir_path.compare("")==0 ){ dir_path.append("/"); }  // If is not root directory append "/"
  dir_path.append(dir_structure.dir_node_name);
  string command_mkdir ="mkdir -p " + dir_path;  // Example: "mkdir -p hola/quetal/comova"
  system(command_mkdir.c_str());
  
  
  // Add files to new directory
  for(vector<ZipHandler::file_st>::iterator it = dir_structure.files_contained.begin() ; it!=dir_structure.files_contained.end() ; ++it) {
    file_st file_contained = (*it);
    string command_cp = "cp " + file_contained.file_id + " " + dir_path + "/" + file_contained.file_real_name; // Example: system("cp 111 hola/quetal/comova/notas.txt");
    system(command_cp.c_str());
  }
  
  
  // Create sub-directories recursively
  for(vector<ZipHandler::dir_tree_node_st>::iterator it = dir_structure.sub_dirs.begin() ; it!=dir_structure.sub_dirs.end() ; ++it) {
    dir_tree_node_st sub_dir = (*it);
    create_file_structure_recursive(dir_path,sub_dir);
  }

  
  return false;
}
