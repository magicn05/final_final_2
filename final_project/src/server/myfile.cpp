#include "myfile.h"
#include <iostream>
#include <string>
using namespace std;

myfile::myfile(){
  this->file_name = "";
  this->file_no = 999;
  this->file_pin = "";
  this->file_size = 999;
}

myfile::myfile(string file_name, int file_no, string file_pin, off_t file_size, string file_mtime){
  this->file_name = file_name;
  this->file_no = file_no;
  this->file_pin = file_pin;
  this->file_size = file_size;
  this->file_mtime = file_mtime;
}

myfile::~myfile(){

}

string myfile::get_file_name(){
  return this->file_name;
}

int myfile::get_file_no(){
  return this->file_no;
}

string myfile::get_file_pin(){
  return this->file_pin;
}

off_t myfile::get_file_size(){
  return this->file_size;
}

string myfile::get_file_time(){
  return this->file_mtime;
}
