#ifndef _REPLY_H_
#define _REPLY_H_
#include <string>
#include <vector>

using namespace std;

class reply {
  private:
  string reply_title;
  string reply_writer;
  string reply_date;
  int reply_cnt=0;
  
  public:
  reply();
  reply(string title, string writer, string date);
  string get_rp_title();
  string get_rp_writer();
  string get_rp_date();
  int get_rp_cnt();
  ~reply();
};

#endif