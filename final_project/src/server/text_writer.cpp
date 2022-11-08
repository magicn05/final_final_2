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
#include <time.h>
using namespace std;

int text_writer(int sd, data_Manager & d_manager, user& now_user){
  char buf_date[30];
  string buf_date_str;
	time_t ct;
	struct tm tm;
	ct = time(NULL);
	tm = *localtime(&ct);
	sprintf(buf_date,"%04d-%02d-%02d %02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,tm.tm_hour, tm.tm_min); //2022-11-05
  
  buf_date_str = buf_date;
   
  char recv_buf[1024];
  char buf[1024];
  int n;
  string my_title;
  string my_contents;
   
    sprintf(buf, "%s", " └ 글 제목을 입력하세요. (최대 80자) >> ");
    send(sd, buf, strlen(buf), 0);
    n = recv(sd, recv_buf, sizeof(recv_buf), 0);
    my_title = recv_buf;
    memset(recv_buf,0,sizeof(recv_buf));
    sprintf(buf, "%s", " └ 글 내용을 입력하세요.(최대 500자) >> ");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", ">> ");
    send(sd, buf, strlen(buf), 0);
    usleep(0.5);
    n = recv(sd, recv_buf, sizeof(recv_buf), 0);
    my_contents = recv_buf;
    memset(recv_buf,0,sizeof(recv_buf));

    sprintf(buf, "%s", " └ 글을 게시하시겠습니까? [Y/n] ");
    send(sd, buf, strlen(buf), 0);
    n = recv(sd, recv_buf, sizeof(recv_buf), 0);
    string temp;
    temp = recv_buf;
    if (temp == "Y"){
    d_manager.add_data(new data(my_title, buf_date_str, now_user.get_userid(), now_user.get_userpin(), my_contents, d_manager.get_data_no()));
    }

  

  return 0;
}