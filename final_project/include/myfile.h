#ifndef _MYFILE_H_
#define _MYFILE_H_

#include <string>
#include <vector>

using namespace std;

class myfile{
private:
  string file_name;
  int file_no;
  string file_pin;
  int file_size;
public:
  myfile();
  myfile(string, int, string, int);
  ~myfile();

  string get_file_name();
  int get_file_no();
  string get_file_pin();
  int get_file_size();
};

#endif