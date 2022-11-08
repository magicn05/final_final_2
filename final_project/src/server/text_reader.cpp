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
  
  
  char printbuf1[100];
  char printbuf2[100];
  char printbuf3[100];
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
    // sprintf(buf, "%s",
    //         "=================================================================="
    //         "===============\n");
    // 
    // send(sd, buf, strlen(buf), 0);
    temp.clear();
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
            "Posted  :         "); /////////////////////////// Posted by
    send(sd, buf, strlen(buf), 0);
    memset(buf,0,sizeof(buf));
    strcpy(buf,d_manager.get_data_date(no).c_str());
    send(sd, buf, strlen(buf), 0);
    memset(buf,0,sizeof(buf));
    sprintf(buf, "%s", "\n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", "=================================================================================\n\n");
    send(sd, buf, strlen(buf), 0);
    temp.clear();

    temp = d_manager.get_data_contents(no);
    temp = temp + '\n';
    temp = temp + '\n';
    
    strcpy(buf, temp.c_str());
    send(sd, buf, strlen(buf), 0); //////////////////////////// Contents
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%s", "=================================================================================\n");
    send(sd, buf, strlen(buf), 0);
    temp.clear();
    sprintf(buf, "%s", "Replies \n"); ///////////////////////////// Replies
    send(sd, buf, strlen(buf), 0);
    usleep(0.5);
    sprintf(buf, "%s",
            " [No.]     [COMMENT]                               [WRITER]       [DATE]      \n");
    send(sd, buf, strlen(buf), 0);

    //cout << "temp_data cnt : " << temp_data->reply_list.size() << endl;

    for (int m=0; m<temp_data->reply_list.size(); m++){
      strcpy(printbuf1,temp_data->get_reply_title(m).c_str());
      strcpy(printbuf2,temp_data->get_reply_owner(m).c_str());
      strcpy(printbuf3,temp_data->get_reply_date(m).c_str());
      sprintf(buf, "   %-5d   %-40s  %-10s  %-20s\n", m+1, printbuf1, printbuf2, printbuf3);
      send(sd, buf, strlen(buf), 0);
      memset(buf, 0, sizeof(buf));
    }
    sprintf(buf, "%s",
            "=================================================================="
            "===============\n\n");
    send(sd, buf, strlen(buf), 0);
    usleep(0.5);
    // sprintf(buf, "%s", " []. 추천하기\n");
    // send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", " [1]. 댓글달기\n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", " [2]. 댓글삭제\n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", " [3]. 돌아가기\n\n");
    send(sd, buf, strlen(buf), 0);
    memset(buf,0,sizeof(buf));
    sprintf(buf, "%s", " Input >> ");
    send(sd, buf, strlen(buf), 0);
    memset(buf, 0, sizeof(buf));
    sleep(1);
    n = recv(sd, recv_buf, sizeof(recv_buf), 0);
    a = atoi(recv_buf);
    switch (a) {
    case 0: //추천하기
      sprintf(buf, "%s", " 추천하였습니다.");
      send(sd, buf, strlen(buf), 0);
      sleep(2);
      sprintf(buf, "%s", "WINDOW");
      send(sd, buf, strlen(buf), 0);
      usleep(0.5);
      break;

    case 1: //댓글 작성
      temp.clear();
      sprintf(buf, "%s", " └ 댓글을 입력하세요 >> ");
      send(sd, buf, strlen(buf), 0);
      memset(recv_buf,0,sizeof(recv_buf));
      n = recv(sd, recv_buf, sizeof(recv_buf), 0);
      temp = recv_buf;
      d_manager.data_list[no]->add_reply(new reply(temp, now_user.get_userid(), buf_date_str));
      temp.clear();
      usleep(0.5);
      break;
    case 2: //댓글 삭제
      temp.clear();
      sprintf(buf, "%s", " └ 삭제하실 댓글 번호를 입력하세요 >> ");
      send(sd, buf, strlen(buf), 0);
      n = recv(sd, recv_buf, sizeof(recv_buf), 0);
      a = atoi(recv_buf);
      a = a - 1;
      sprintf(buf, "%s", " └ 정말 삭제 하시겠습니까? [Y/n] >> ");
      send(sd, buf, strlen(buf), 0);
      usleep(0.5);
      memset(recv_buf,0,sizeof(recv_buf));
      n = recv(sd, recv_buf, sizeof(recv_buf), 0);
      if(strcmp(recv_buf,"Y")==0){
        if(now_user.get_userid() == d_manager.data_list[no]->reply_list[a]->get_rp_writer())
          {
            //v.erase(v.begin() + i);
            d_manager.data_list[no]->reply_list.erase(d_manager.data_list[no]->reply_list.begin() + a);
            //cout << " 댓글 작성자 검색" << endl;
          }
        else{
          sprintf(buf, "%s", " Error :: 작성자가 불일치하여 삭제를 취소합니다. ");
          send(sd, buf, strlen(buf), 0);
          sleep(3);
          break;
        }
      }
      else{
        sprintf(buf, "%s", " Error :: 댓글 삭제를 취소합니다. ");
        send(sd, buf, strlen(buf), 0);
        sleep(3);
      }
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