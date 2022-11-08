#ifndef _MYFILE_H_
#define _MYFILE_H_

#include <string>
#include <vector>
#include <time.h>

using namespace std;

class myfile{
private:
  string file_name;
  int file_no;
  string file_pin;
  off_t file_size;
  string file_mtime;
public:
  myfile();
  myfile(string, int, string, off_t, string);
  ~myfile();

  string get_file_name();
  int get_file_no();
  string get_file_pin();
  off_t get_file_size();
  string get_file_time();
};

#endif