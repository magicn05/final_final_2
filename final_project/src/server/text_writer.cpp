#include <iostream>
#include <string>
#include <vector>
#include "user.h"
#include "user_manager.h"
#include "data_manager.h"
#include "data.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
using namespace std;

int text_writer(int sd, data_Manager & d_manager){
  char recv_buf[1024];
  char buf[1024];
  int n;
  string my_title;
  string my_contents;
  string my_date = "2022-10-10"; //ioctl 
  string my_owner = "banana";
  string my_pin = "1234";
  int postno = 10;

  
    sprintf(buf, "%s", "글 제목을 입력하세요 : \n");
    send(sd, buf, strlen(buf), 0);
    n = recv(sd, recv_buf, sizeof(recv_buf), 0);
    my_title = recv_buf;
    memset(recv_buf,0,sizeof(recv_buf));

    sprintf(buf, "%s", "글 내용을 입력하세요 : \n");
    send(sd, buf, strlen(buf), 0);
    n = recv(sd, recv_buf, sizeof(recv_buf), 0);
    my_contents = recv_buf;
    memset(recv_buf,0,sizeof(recv_buf));

    sprintf(buf, "%s", "글을 게시하시겠습니까? [Y/n] ");
    send(sd, buf, strlen(buf), 0);
    n = recv(sd, recv_buf, sizeof(recv_buf), 0);
    string temp;
    temp = recv_buf;
    if (temp == "Y"){
    d_manager.add_data(new data(my_title, my_date, my_owner, my_pin, my_contents, 101));
    }

  

  return 0;
}