#ifndef _REPLY_H_
#define _REPLY_H_
#include <string>
#include <vector>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

using namespace std;

class reply {
  private:
  string reply_title;
  string reply_writer;
  string reply_date;
  int reply_cnt=0;
  
  friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        // serialize base class information
        ar & reply_title;
        ar & reply_writer;
        ar & reply_date;
        ar & reply_cnt;
    }

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