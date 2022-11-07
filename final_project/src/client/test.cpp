#include <iostream>
#include <cstring>
#include <vector>
#include <string>
using namespace std;

class user{
  string user_id;
  string user_pin;
  string user_name;
  string chat_name;
  public:
  user(string user_id, string user_pin, string user_name, string chat_name);
  ~user(){};
  string get_userid(){return user_id;}
};

user::user(string user_id="", string user_pin="", string user_name="", string chat_name=""){
  this->user_id = user_id;
  this->user_pin = user_pin;
  this->user_name = user_name;
  this->chat_name = chat_name;
}

class user_Manager {

public:
  vector<user*> user_list;
  void add_user(user* new_user);
};

void user_Manager::add_user(user* new_user)
{ user_list.push_back(new_user); }

int main()
{
  

  // char buf[1024];
  // string s;
  // getline(cin, s);
  // cout << "length of s : " << s.length() << endl;
  // s = s + '\0';
  // cout << "length of s : " << s.length() << endl;
  // sprintf(buf,"%s", "window");
  // cout << buf << endl;
  // if(strcmp(buf, "window")==0)
  //   cout << "equal" << endl;
  
  user_Manager u_manager;
  // u_manager.add_user(new user("kim", "1234", "1234", "david"));
  // cout << u_manager.user_list[0].get_userid() << endl;

  // user kim("kim","1234");
  // u_manager.add_user(kim);
  // cout << u_manager.user_list[0].get_userid() << endl;

  // user* temp_user;
  // temp_user = new user("lee", "1234", "1234", "david");
  // u_manager.add_user(*temp_user);
  // cout << u_manager.user_list[0].get_userid() << endl;
  // user_list.push_back(new user("park","1234","kkk","david"));
  // cout << user_list[0]->get_userid() << endl;
  u_manager.add_user(new user("park","1234","kkk","david"));
  cout << u_manager.user_list[0]->get_userid() << endl;
}