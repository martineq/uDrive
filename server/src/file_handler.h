#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <iostream>
#include <stdio.h>
#include <string>

using namespace std;

class FileHandler {

  public:
    FileHandler();
    ~FileHandler();

    /**
     * @brief Reads a file saved previously
     * 
     * @param file_name ...
     * @param p_file_stream retunrs the file stream
     * @return size_t the size of the file readed (in Bytes)
     */
    size_t load_file(string file_name, char*& p_file_stream);
    
    /**
     * @brief Saves a file 
     * 
     * @param file_name ...
     * @param p_file_stream ...
     * @param size_stream ...
     * @return size_t the size of the file readed (in Bytes)
     */
    size_t save_file(string file_name, const char* p_file_stream, size_t size_stream); 

    /**
     * @brief Deletes the file from disk
     * 
     * @param file_name ...
     * @return void
     */
    void delete_file(string file_name);
    
};

#endif // FILEHANDLER_H
