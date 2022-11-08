#ifndef _DATA_H_
#define _DATA_H_
#include <string>
#include <iostream>
#include <vector>
#include "reply.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

using namespace std;

class data{
  private:
  string contents;
  string data_passwd;
  int postno;
  string data_title; //reply
  string data_date; //reply
  string data_owner; //reply
  
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        // serialize base class information
        ar & contents;
        ar & data_passwd;
        ar & postno;
        ar & data_title;
        ar & data_date;
        ar & data_owner;
        ar & reply_list;
  
    }

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