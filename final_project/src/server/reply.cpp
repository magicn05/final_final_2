#include "reply.h"
#include <iostream>
#include <string>

using namespace std;

reply::reply(){
  this->reply_writer="";
  this->reply_date="";
  this->reply_title="";
}

reply::reply(string title, string writer, string date){
  this->reply_title = title;
  this->reply_writer = writer;
  this->reply_date = date;
  reply_cnt ++;
}

string reply::get_rp_title(){
  return this->reply_title;
}

int reply::get_rp_cnt(){
  return reply_cnt;
}

string reply::get_rp_writer(){
  return reply_writer;
}

string reply::get_rp_date(){
  return reply_date;
}

reply::~reply(){
}