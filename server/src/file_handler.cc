#include "file_handler.h"

FileHandler::FileHandler(){

}

FileHandler::~FileHandler(){

}

size_t FileHandler::load_file(string user_id, string file_id, string revision, char*& p_file_stream){
  
  FILE * p_file;
  string file_name = user_id+file_id+revision;

  p_file = fopen(file_name.c_str(),"rb");
  if(p_file==NULL){
    std::cerr <<"File reading error." << std::endl;
    return 0;
  }

  // Obtain the file size
  fseek(p_file,0,SEEK_END);
  long int size_of_file = ftell(p_file);
  rewind(p_file);

  // Allocate memory for whole file
  p_file_stream = (char*) malloc (sizeof(char)*size_of_file);
  if (p_file_stream == NULL){
    std::cerr <<"File memory allocate error." << std::endl;
    return 0;
  }

  // Copy the file
  size_t result = fread(p_file_stream,1,size_of_file,p_file);

  fclose(p_file);
  
  if (result != size_of_file){
    std::cerr <<"File reading error." << std::endl;
    return 0;
  }

  return result;
}

size_t FileHandler::save_file(string user_id, string file_id, string revision, const char* p_file_stream, size_t p_size_stream){

  FILE * p_file;
  string file_name = user_id+file_id+revision;

  p_file = fopen(file_name.c_str(),"wb");
  if(p_file==NULL){
    std::cerr <<"File reading error." << std::endl;
    return 0;
  }

  size_t size_write = fwrite(p_file_stream,sizeof(char),p_size_stream,p_file);

  fclose(p_file);

  if (size_write != p_size_stream){ std::cerr <<"File reading error." << std::endl; }
  
  return size_write;
}