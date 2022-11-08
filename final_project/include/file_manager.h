#ifndef _FILE_MANAGERS_H
#define _FILE_MANAGERS_H

#include "myfile.h"
#include <string>
#include <vector>
#include <time.h>
using namespace std;

class file_Manager{
private:
  int file_cnt;
public:
  vector<myfile*> file_list;
  file_Manager();
  ~file_Manager();
  void add_file(myfile* myfile);
  int get_file_cnt();
  int get_file_postno(int i);
  string get_file_title(int i);
  string get_file_passwd(int i);
  off_t get_file_size_(int i);
  string get_file_time_(int i);
  void list_clear();
  
};
#endif
