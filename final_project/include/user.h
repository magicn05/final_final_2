#ifndef _USER_H
#define _USER_H

#include <string>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

using namespace std;

class user {
  string user_id;
  string user_pin;
  string user_name;
  string phone_number;
  friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        // serialize base class information
        ar & user_id;
        ar & user_pin;
        ar & user_name;
        ar & phone_number;
    }
public:
  user();
  user(string user_id, string user_pin, string user_name, string phone_number);
  ~user();
  string get_userid();
  string get_userpin();
  string get_userName();
  string get_userPhone();
  void set_userid(string user_id);
  void set_userpin(string user_pin);
  void set_userName(string user_Name);
  void set_userPhone(string user_Phone);
};

#endif