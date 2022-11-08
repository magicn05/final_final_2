#include "data.h"
#include "user_manager.h"
#include "data_manager.h"
#include "user.h"
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
using namespace std;

int text_reader(int sd, int n, data_Manager &d_manager, data* temp_data, user &now_user);
int text_writer(int sd, data_Manager &d_manager, user&now_user);

int board(int sd, data_Manager &d_manager, pthread_mutex_t &mutx, user &now_user) {
  char printbuf[100];
  char printbuf2[100];
  char printbuf3[100];
  char buf_date[30];
  string buf_date_str;
	time_t ct;
	struct tm tm;
	ct = time(NULL);
	tm = *localtime(&ct);
	sprintf(buf_date,"%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday); //2022-11-05
  buf_date_str = buf_date;
  size_t found;
  vector<int> searched_index;
  data* temp_data;
  int end_flag = 0;
  string temp;
  char recv_buf[2048];
  char buf[1024];
  int a, b, n;
  int search_flag = 0;
  int board_page = 0;
  int current_page = 1;
  while (end_flag != 1) {
    board_page = (d_manager.get_data_cnt()/10)+1;
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%s", "WINDOW");
    send(sd, buf, strlen(buf), 0);
    usleep(0.5);
    memset(buf,0,sizeof(buf));
    sprintf(buf, "%s", " 게시판입니다. 여러분의 의견을 공유하세요. \n\n\n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s",
            "                         B U L L E T I N         B O A R D \n");
    send(sd, buf, strlen(buf), 0);
    usleep(0.5);
    if(search_flag != 1){
    sprintf(buf, "                                                                     %d %s %d %s", current_page, "page of ", board_page, "pages\n");
    send(sd, buf, strlen(buf), 0);
    }
    sprintf(buf, "%s",
            "=========================================================================================\n");
    send(sd, buf, strlen(buf), 0);
    
    sprintf(buf, "%s",
            " [Post No.]         [TITLE]                     [WRITER]                  [DATE]      \n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s",
            "=================================================================="
            "=======================\n");
    send(sd, buf, strlen(buf), 0);
    usleep(0.5);
    if(search_flag == 0){
      
      if (current_page != board_page){
          for (int i = (d_manager.get_data_cnt() - ((current_page-1)*10)); i >= (d_manager.get_data_cnt() - ((current_page-1)*10) - 9) ; i--)
          {
            memset(buf, 0, sizeof(buf));
            strcpy(printbuf,d_manager.get_data_title(i-1).c_str());
            strcpy(printbuf2,d_manager.get_data_owner(i-1).c_str());
            strcpy(printbuf3,d_manager.get_data_date(i-1).c_str());
            sprintf(buf, "   %-5d           %-30s %-15s    %-20s\n", d_manager.get_data_postno(i-1), printbuf, printbuf2, printbuf3);  
            send(sd, buf, strlen(buf), 0);
            usleep(0.5);
          }
         }
      else if (current_page == board_page){
         for (int i = d_manager.get_data_cnt() - ((current_page-1)*10); i >= 0 ; i--)
          {
            memset(buf, 0, sizeof(buf));
            strcpy(printbuf,d_manager.get_data_title(i).c_str());
            strcpy(printbuf2,d_manager.get_data_owner(i).c_str());
            strcpy(printbuf3,d_manager.get_data_date(i).c_str());
            sprintf(buf, "   %-5d           %-30s %-15s    %-20s\n", d_manager.get_data_postno(i), printbuf, printbuf2, printbuf3);  
            send(sd, buf, strlen(buf), 0);
            usleep(0.5);
           }
        } 
      }

    else if (search_flag == 1) {
      memset(buf, 0, sizeof(buf));
      sprintf(buf, "%s", " 현재창에는 검색하신 결과만 나타납니다.(등록순) \n");
      send(sd, buf, strlen(buf), 0);

      for (int i = 0; i < searched_index.size(); i++) 
      {
        memset(buf, 0, sizeof(buf));
        strcpy(printbuf,d_manager.get_data_title(searched_index[i]).c_str());
        strcpy(printbuf2,d_manager.get_data_owner(searched_index[i]).c_str());
        strcpy(printbuf3,d_manager.get_data_date(searched_index[i]).c_str());
        sprintf(buf, "     %-10d    %-30s%-20s    %-20s\n", d_manager.get_data_postno(searched_index[i]), printbuf, printbuf2, printbuf3);  
        send(sd, buf, strlen(buf), 0);
        usleep(0.5);
      //  temp.clear();
      }
    }
    sprintf(buf, "%s",
            "=================================================================="
            "=======================\n");
    send(sd, buf, strlen(buf), 0);
    usleep(0.5);
    sprintf(buf, "%s", " [0]. 처음페이지로(새로고침) \n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", " [1]. 글 읽기\n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", " [2]. 글 작성\n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", " [3]. 글 삭제\n");
    send(sd, buf, strlen(buf), 0);
    if (search_flag != 1){
    sprintf(buf, "%s", " [4]. 글 검색\n");
    send(sd, buf, strlen(buf), 0);
    }
    sprintf(buf, "%s", " [5]. 다음페이지로\n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", " [6]. 이전페이지로\n");
    send(sd, buf, strlen(buf), 0);
    if (search_flag == 1){
    sprintf(buf, "%s", " [7]. 검색모드해제 ◀\n");
    send(sd, buf, strlen(buf), 0);
    }
    sprintf(buf, "%s", " [9]. 나가기\n\n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", " Input >> ");
    send(sd, buf, strlen(buf), 0);
    memset(buf, 0, sizeof(buf));
    usleep(0.5);
    n = recv(sd, recv_buf, sizeof(recv_buf), 0);
    //recv_buf[n] = '\0';
    a = atoi(recv_buf);
    switch (a) {
    case 0:
      sprintf(buf, "%s", "WINDOW");
      send(sd, buf, strlen(buf), 0);
      current_page = 1;
      sleep(1);
      break;

    case 1: //긁 일기 + 댓글 작성
          //pthread_mutex_lock(&mutx);    
          int select;
          sprintf(buf, "%s", " └ 조회하고 싶은 글의 번호를 입력하세요 >> ");
          send(sd, buf, strlen(buf), 0);
          n = recv(sd, recv_buf, sizeof(recv_buf), 0);
          a = atoi(recv_buf);
          cout << "a : " << a << endl;
          cout << "d_manager.get_data_cnt() : " << d_manager.get_data_cnt() << endl;
          
          //cout << " idk : " << d_manager.get_data_postno(d_manager.get_data_cnt()) << endl;
          //cout << d_manager.get_data_postno(d_manager.get_data_cnt() - 1)
          if(a > d_manager.get_data_postno(d_manager.get_data_cnt() - 1)){
          
            memset(buf,0,sizeof(buf));
            sprintf(buf, "%s", " Error : 입력이 잘못되었습니다.!(최대치초과) \n");
            send(sd, buf, strlen(buf), 0);
            sleep(1);
            break;
          }
          else{
          for (int i = 0; i < d_manager.get_data_cnt(); i++) {
            if ((d_manager.get_data_postno(i)) == a) {
              select = i;
            }
          }
          temp_data = d_manager.data_list[select];
          text_reader(sd, a, d_manager, temp_data, now_user);
          memset(recv_buf, 0, sizeof(recv_buf));
          }
          //pthread_mutex_unlock(&mutx);
      break;

    case 2: // 새글 작성
          sprintf(buf, "%s", " └ 새글을 작성하시겠습니까? [Y/n] ");
          send(sd, buf, strlen(buf), 0);
          n = recv(sd, recv_buf, sizeof(recv_buf), 0);
          text_writer(sd, d_manager, now_user);
      break;

    case 3: //
        memset(recv_buf, 0, sizeof(recv_buf));
        sprintf(buf, "%s", " └ 삭제하고 싶은 글의 번호를 입력하세요. >> ");
        send(sd, buf, strlen(buf), 0);
        n = recv(sd, recv_buf, sizeof(recv_buf), 0);
        a = atoi(recv_buf);
        for (int i = 0; i < d_manager.get_data_cnt(); i++) {
          if ((d_manager.get_data_postno(i)) == a) {
            select = i;
          }
        }
        if( d_manager.get_data_owner(select) == now_user.get_userid())
        {
          (d_manager.data_list).erase((d_manager.data_list).begin()+select);
          d_manager.down_data_cnt();
          sprintf(buf, "%s", " Success :: 해당 글이 삭제가 되었습니다.");
          send(sd, buf, strlen(buf), 0);
          sleep(3);
          memset(recv_buf, 0, sizeof(recv_buf));
        }
        else{
          sprintf(buf, "%s", " Error :: 다른 작성자의 글은 삭제할수 없습니다.");
          send(sd, buf, strlen(buf), 0);
          sleep(3);
        }
      break;

    case 4:
        temp.clear();
        searched_index.clear();
        memset(recv_buf, 0, sizeof(recv_buf));
        sprintf(buf, "%s", " └ 검색하고 싶은 글의 제목 키워드를 입력하세요. >> ");
        send(sd, buf, strlen(buf), 0);
        n = recv(sd, recv_buf, sizeof(recv_buf), 0);
        temp = recv_buf;
        //cout << "temp : " << temp << endl;
        for(int k=0; k<d_manager.get_data_cnt(); k++){
          size_t found = d_manager.get_data_title(k).find(temp);
          if (found!=string::npos){
            searched_index.push_back(k);
          }
        }
        if(searched_index.size() == 0){
        sprintf(buf, "%s", " └ 찾고자 하는 글이 없습니다. \n");
        send(sd, buf, strlen(buf), 0);
        sleep(3);
        }
        else if (searched_index.size() != 0){
          sprintf(buf, "%s %ld %s", " └ 총 ",searched_index.size(), " 개의 글을 찾았습니다. \n");
          send(sd, buf, strlen(buf), 0);
          sleep(2); 
          sprintf(buf, "%s %ld %s", " └ 총 ",searched_index.size(), " 검색한 글만 보시겠습니까? [Y/n] >> ");
          send(sd, buf, strlen(buf), 0);
          memset(recv_buf, 0, sizeof(recv_buf));
          n = recv(sd, recv_buf, sizeof(recv_buf), 0);
          if (strcmp(recv_buf,"Y")==0){
            search_flag = 1;
          }
        }
        cout << "searched_index length : " << searched_index.size() << endl;
        temp.clear();
      break;
    case 5: //next page;
      if (current_page == board_page){
        memset(buf,0,sizeof(buf));
        sprintf(buf, "%s", " Error : 마지막 페이지 입니다.! \n");
        send(sd, buf, strlen(buf), 0);
        sleep(2);
      }
      else 
        current_page = current_page + 1;
      break;
    case 6:
      if (current_page != 1)
        current_page = current_page - 1;
      else{ 
        memset(buf,0,sizeof(buf));
        sprintf(buf, "%s", " Error : 처음 페이지 입니다.! \n");
        send(sd, buf, strlen(buf), 0);
        sleep(2);
      }
      break;

    case 7:
      search_flag = 0;
      searched_index.clear();
    break;
    case 9:
      end_flag = 1;
      break;
    }
    usleep(0.5);
    memset(recv_buf, 0, sizeof(recv_buf));
  }

  return 0;
}