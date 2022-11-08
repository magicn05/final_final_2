#include <iostream>
#include <string>
#include "user_manager.h"
#include "data_manager.h"
#include "user.h"
#include "data.h"
#include "file_manager.h"
#include "myfile.h"
#include "reply.h"
using namespace std;

user_Manager::user_Manager(){
  user_cnt = 0;  
}

user_Manager::~user_Manager(){
}

void user_Manager::add_user(user* new_user){
  user_list.push_back(new_user);
  user_cnt++;
}

void user_Manager::get_user_id(int n){
    cout << user_list[n]->get_userid() << endl;
}

void user_Manager::get_user_name(int n){
    cout << user_list[n]->get_userName() << endl;
}

void user_Manager::get_user_pin(int n){
    cout << user_list[n]->get_userpin() << endl;
}

void user_Manager::get_user_phoneNum(int n){
    cout << user_list[n]->get_userPhone() << endl;
}

int user_Manager::get_user_cnt(){
  return user_cnt;
}

int user_Manager::check_login(string id, string pin){
  //cout << "check point " << endl;
  //cout << "input id : " << id << endl;
  //cout << "input pin : " << pin << endl;
  int check_flag = 0;
  for (int i=0; i<this->user_cnt; i++){
    if((user_list[i]->get_userid()) == id){
      if ((user_list[i]->get_userpin()) == pin){
        check_flag = 1;
      }
    }
  }
  if (check_flag == 1)
    return 0; //성공
  else
    return 1; //실패
}

data_Manager::data_Manager(){
   int data_cnt = 0;
   int data_no = 10;
}

data_Manager::~data_Manager(){
  
}

void data_Manager::add_data(data *new_data) {
  data_list.push_back(new_data);
  data_cnt++;
  data_no++;
}

string data_Manager::get_data_title(int n) { return data_list[n]->get_title(); }

string data_Manager::get_data_owner(int n) { return data_list[n]->get_owner(); }

string data_Manager::get_data_passwd(int n) {
  return data_list[n]->get_passwd();
}

string data_Manager::get_data_date(int n) { return data_list[n]->get_date(); }
int data_Manager::get_data_cnt() { return data_cnt; }

string data_Manager::get_data_contents(int n){
  return data_list[n]->get_contents();
}

data* data_Manager::get_data_total(int n){
  return data_list[n];
}

int data_Manager::get_data_postno(int n){
  return data_list[n]->get_postno();
}

void data_Manager::put_data_reply(int n, reply* reply_){
  data_list[n]->add_reply(reply_);
}

void data_Manager::down_data_cnt(){
  this->data_cnt = this->data_cnt - 1;
}

file_Manager::file_Manager(){
  file_cnt = 0;
}

file_Manager::~file_Manager(){

}

void file_Manager::add_file(myfile* myfile){
  file_list.push_back(myfile);
  file_cnt++;
}

int data_Manager::get_data_no(){
  return data_no;
}

int file_Manager::get_file_cnt(){
  return file_cnt;
}

int file_Manager::get_file_postno(int i){
  return file_list[i]->get_file_no();
}

string file_Manager::get_file_title(int i){
  return file_list[i]->get_file_name();
}

string file_Manager::get_file_passwd(int i){
  return file_list[i]->get_file_pin();
}

off_t file_Manager::get_file_size_(int i){
  return file_list[i]->get_file_size();
}

string file_Manager::get_file_time_(int i){
  return file_list[i]->get_file_time();
}

void file_Manager::list_clear(){
  file_list.clear();
  file_cnt = 0;
  //f_no = 0;


}