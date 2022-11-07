#ifndef _DATA_MANAGERS_H
#define _DATA_MANAGERS_H

#include "user.h"
#include "data.h"
#include "reply.h"
#include <string>
#include <vector>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

using namespace std;

class data_Manager{
private:
  int data_cnt;
  int data_no;
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        // serialize base class information
        ar & data_cnt;
        ar & data_no;
        ar & data_list;
    }
public:
  vector<data*> data_list;
  data_Manager();
  ~data_Manager();
  void add_data(data *new_data);
  string get_data_title(int n);
  string get_data_owner(int n);
  string get_data_passwd(int n);
  string get_data_date(int n);
  string get_data_contents(int n);
  data* get_data_total(int n);
  int get_data_cnt();
  int get_data_postno(int n);
  void put_data_reply(int n, reply* reply_);
  void down_data_cnt();
  int get_data_no();
  
  
};
#endif
