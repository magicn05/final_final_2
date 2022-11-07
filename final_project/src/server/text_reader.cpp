#include "data.h"
#include "user_manager.h"
#include "data_manager.h"
#include "user.h"
#include "reply.h"
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <time.h>
#include <stdio.h>
using namespace std;

int text_reader(int sd, int selection, data_Manager &d_manager, data* temp_data, user &now_user) {
  char buf_date[30];
  string buf_date_str;
	time_t ct;
	struct tm tm;
	ct = time(NULL);
	tm = *localtime(&ct);
	sprintf(buf_date,"%04d-%02d-%02d %02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,tm.tm_hour, tm.tm_min);
  buf_date_str = buf_date;
  
  
  //cout << temp_data->get_contents() << endl;
  string temp, reply_temp;
  char buf[1024];
  char recv_buf[1024];
  int a;
  int end_flag = 0;
  sprintf(buf, "%s", "WINDOW");
  send(sd, buf, strlen(buf), 0);
  usleep(0.5);
  cout << "selection : " << selection << endl;
  int n;
  int no ;
  for(int i=0; i<d_manager.get_data_cnt(); i++){
    if ((d_manager.get_data_postno(i)) == selection){
      no = i;
    }
  }

  
  while (end_flag != 1) {
    usleep(0.5);  
    sprintf(buf, "%s", "WINDOW");
    send(sd, buf, strlen(buf), 0);
    usleep(0.5);  
    sprintf(buf, "%s",
            "=================================================================="
            "===============\n");
    send(sd, buf, strlen(buf), 0);
    memset(buf, 0, sizeof(buf));

    sprintf(buf, "%s",
            "Title : "); //////////////////////////// Title
    send(sd, buf, strlen(buf), 0);
    memset(buf, 0, sizeof(buf));

    temp = "            " + d_manager.get_data_title(no);
    temp = temp + '\n';
    strcpy(buf, temp.c_str());
    send(sd, buf, strlen(buf), 0);
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%s",
            "=================================================================="
            "===============\n");
    temp.clear();
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s",
            "Written by : "); /////////////////////////// Written by
    send(sd, buf, strlen(buf), 0);
    memset(buf, 0, sizeof(buf));
    temp = "            " + d_manager.get_data_owner(no);
    temp = temp + '\n';
    strcpy(buf, temp.c_str());
    send(sd, buf, strlen(buf), 0);
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%s",
            "=================================================================="
            "===============\n\n");
    send(sd, buf, strlen(buf), 0);
    temp.clear();

    temp = d_manager.get_data_contents(no);
    temp = temp + '\n';
    temp = temp + '\n';
    //buf[temp.size()] = '\n';
    //buf[temp.size()] = '\n';
    
    strcpy(buf, temp.c_str());
    send(sd, buf, strlen(buf), 0); //////////////////////////// Contents
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%s",
            "=================================================================="
            "===============\n");
    send(sd, buf, strlen(buf), 0);
    temp.clear();
    sprintf(buf, "%s", "Replies \n"); ///////////////////////////// Replies
    send(sd, buf, strlen(buf), 0);
    usleep(0.5);

    cout << "temp_data cnt : " << temp_data->reply_list.size() << endl;

    for (int m=0; m<temp_data->reply_list.size(); m++){
      reply_temp = "  ";
      reply_temp = reply_temp + to_string(m+1);
      reply_temp = reply_temp + "    ";
      reply_temp = reply_temp + temp_data->get_reply_title(m);
      reply_temp = reply_temp + "    ";
      reply_temp = reply_temp + temp_data->get_reply_owner(m);
      reply_temp = reply_temp + "    ";
      reply_temp = reply_temp + temp_data->get_reply_date(m);
      reply_temp = reply_temp + '\n';
      //cout << "reply : " << reply_temp << endl;
      
      strcpy(buf, reply_temp.c_str());
      send(sd, buf, strlen(buf), 0);
      memset(buf, 0, sizeof(buf));
      reply_temp.clear();
    }
    sprintf(buf, "%s",
            "=================================================================="
            "===============\n\n");
    send(sd, buf, strlen(buf), 0);
    usleep(0.5);
    sprintf(buf, "%s", "[1]. 추천하기\n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", "[2]. 댓글달기\n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", "[3]. 돌아가기\n");
    send(sd, buf, strlen(buf), 0);
    usleep(0.5);
    n = recv(sd, recv_buf, sizeof(recv_buf), 0);
    a = atoi(recv_buf);
    switch (a) {
    case 1: //추천하기
      sprintf(buf, "%s", "추천하였습니다.");
      send(sd, buf, strlen(buf), 0);
      sleep(2);
      sprintf(buf, "%s", "WINDOW");
      send(sd, buf, strlen(buf), 0);
      usleep(0.5);
      break;

    case 2: //댓글 작성
      temp.clear();
      sprintf(buf, "%s", "댓글을 입력하세요 >> ");
      send(sd, buf, strlen(buf), 0);
      memset(recv_buf,0,sizeof(recv_buf));
      n = recv(sd, recv_buf, sizeof(recv_buf), 0);
      temp = recv_buf;
      d_manager.data_list[no]->add_reply(new reply(temp, now_user.get_userid(), buf_date_str));
      temp.clear();
      usleep(0.5);
      break;

    case 3: // 돌아가기
      end_flag = 1;
      sprintf(buf, "%s", "WINDOW");
      send(sd, buf, strlen(buf), 0);
      usleep(0.5);
      break;
    }
  }

  return 0;
}