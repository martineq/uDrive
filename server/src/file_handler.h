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

    size_t load_file(string user_id, string file_id, string revision, char*& p_file_stream);
    size_t save_file(string user_id, string file_id, string revision, const char* p_file_stream, size_t p_size_stream); 

};

#endif // FILEHANDLER_H
