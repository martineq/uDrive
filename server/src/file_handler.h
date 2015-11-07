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
     * @brief ...
     * 
     * @param file_name ...
     * @param p_file_stream ...
     * @return size_t
     */
    size_t load_file(string file_name, char*& p_file_stream);
    
    /**
     * @brief ...
     * 
     * @param file_name ...
     * @param p_file_stream ...
     * @param size_stream ...
     * @return size_t
     */
    size_t save_file(string file_name, const char* p_file_stream, size_t size_stream); 

};

#endif // FILEHANDLER_H
