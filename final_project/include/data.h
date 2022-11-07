#ifndef _DATA_H_
#define _DATA_H_
#include <string>
#include <iostream>
#include <vector>
#include "reply.h"
using namespace std;

class data{
  private:
  string contents;
  string data_passwd;
  int postno;
  string data_title; //reply
  string data_date; //reply
  string data_owner; //reply
  
  
  public:
  vector<reply*> reply_list;
  data();
  data(string data_title, string data_date, string data_owner, string data_passwd, string contents, int postno);
  int get_postno();
  string get_title();
  string get_date();
  string get_owner();
  string get_passwd();
  string get_contents();  
  //string get_reply(int i);
  //void add_reply(string reply_text);
  //int get_replyno();
  
  void add_reply(reply* reply_);
  string get_reply_owner(int n);
  string get_reply_title(int n);
  string get_reply_date(int n);
  //void get_reply_cnt();
  ~data();

};

#endif