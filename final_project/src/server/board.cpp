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
using namespace std;

int text_reader(int sd, int n, data_Manager &d_manager, data* temp_data);
int text_writer(int sd, data_Manager &d_manager);

int board(int sd, data_Manager &d_manager, pthread_mutex_t &mutx) {
  vector<int> searched_index;
  data* temp_data;
  int end_flag = 0;
  string temp;
  char recv_buf[2048];
  char buf[1024];
  int a, b, n;
  int search_flag = 0;
  while (end_flag != 1) {
    
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%s", "WINDOW");
    send(sd, buf, strlen(buf), 0);
    usleep(0.5);
    
    sprintf(buf, "%s", "게시판입니다. 여러분의 의견을 공유해주세요. \n\n");
    send(sd, buf, strlen(buf), 0);
    usleep(0.5);
    
    sprintf(buf, "%s",
            "=================================================================="
            "===============\n");
    send(sd, buf, strlen(buf), 0);
    
    sprintf(buf, "%s",
            " [Post No.]                 [TITLE]                    [WRITER]       "
            "[DATE]           [READ]\n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s",
            "=================================================================="
            "===============\n");
    send(sd, buf, strlen(buf), 0);
    usleep(0.5);
    if(search_flag == 0){
      for (int i = 0; i < d_manager.get_data_cnt(); i++) {
        memset(buf, 0, sizeof(buf));
        temp = temp + "   ";
        temp = temp + to_string(d_manager.get_data_postno(i));
        temp = temp + "                  ";
       temp = temp + d_manager.get_data_title(i);
       temp = temp + "                       ";
       temp = temp + d_manager.get_data_owner(i);
       temp = temp + "        ";
       temp = temp + d_manager.get_data_date(i);
      //cout << "temp : " << temp << endl;
        strcpy(buf, temp.c_str());
        buf[temp.size()] = '\n';
       send(sd, buf, strlen(buf), 0);
       usleep(0.5);
       temp.clear();
      }
    }
    else if (search_flag == 1){
      memset(buf, 0, sizeof(buf));
      sprintf(buf, "%s", "현재창에는 검색하신 결과만 나타납니다. \n");
      send(sd, buf, strlen(buf), 0);

      for (int i = 0; i < searched_index.size(); i++) {
      memset(buf, 0, sizeof(buf));
      temp = temp + "   ";
      temp = temp + to_string(d_manager.get_data_postno(searched_index[i]));
      temp = temp + "                  ";
      temp = temp + d_manager.get_data_title(searched_index[i]);
      temp = temp + "                       ";
      temp = temp + d_manager.get_data_owner(searched_index[i]);
      temp = temp + "        ";
      temp = temp + d_manager.get_data_date(searched_index[i]);
      //cout << "temp : " << temp << endl;
        strcpy(buf, temp.c_str());
        buf[temp.size()] = '\n';
       send(sd, buf, strlen(buf), 0);
       usleep(0.5);
       temp.clear();
      }
    }
    sprintf(buf, "%s",
            "=================================================================="
            "===============\n");
    send(sd, buf, strlen(buf), 0);
    usleep(0.5);
    sprintf(buf, "%s", "[0]. 새로고침\n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", "[1]. 글 읽기\n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", "[2]. 글 작성\n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", "[3]. 글 삭제\n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", "[4]. 글 검색\n");
    send(sd, buf, strlen(buf), 0);

    if (search_flag == 1){
    sprintf(buf, "%s", "[5]. 검색모드해제\n");
    send(sd, buf, strlen(buf), 0);
    }

    sprintf(buf, "%s", "[9]. 나가기\n");
    send(sd, buf, strlen(buf), 0);


    usleep(0.5);
    n = recv(sd, recv_buf, sizeof(recv_buf), 0);
    //recv_buf[n] = '\0';
    a = atoi(recv_buf);
    switch (a) {
    case 0:
      sprintf(buf, "%s", "WINDOW");
      send(sd, buf, strlen(buf), 0);
      usleep(0.5);
      break;

    case 1: //긁 일기 + 댓글 작성
          //pthread_mutex_lock(&mutx);    
          int select;
          sprintf(buf, "%s", "조회하고 싶은 글의 번호를 입력하세요 >> ");
          send(sd, buf, strlen(buf), 0);
          n = recv(sd, recv_buf, sizeof(recv_buf), 0);
          a = atoi(recv_buf);

          for (int i = 0; i < d_manager.get_data_cnt(); i++) {
            if ((d_manager.get_data_postno(i)) == a) {
              select = i;
            }
          }
          temp_data = d_manager.data_list[select];
          
          text_reader(sd, a, d_manager, temp_data);
          
          memset(recv_buf, 0, sizeof(recv_buf));
          //pthread_mutex_unlock(&mutx);
      break;

    case 2: // 새글 작성
          sprintf(buf, "%s", "새글을 작성하시겠습니까? [Y/n] ");
          send(sd, buf, strlen(buf), 0);
          n = recv(sd, recv_buf, sizeof(recv_buf), 0);
          text_writer(sd, d_manager);
      break;

    case 3: //
        memset(recv_buf, 0, sizeof(recv_buf));
        sprintf(buf, "%s", "삭제하고 싶은 글의 번호를 입력하세요");
        send(sd, buf, strlen(buf), 0);
        // sprintf(buf, "%s", "남의글을 지울때는 비밀번호를 알아야 합니다.");
        // send(sd, buf, strlen(buf), 0);
        n = recv(sd, recv_buf, sizeof(recv_buf), 0);
        a = atoi(recv_buf);
        for (int i = 0; i < d_manager.get_data_cnt(); i++) {
          if ((d_manager.get_data_postno(i)) == a) {
            select = i;
          }
        }
        (d_manager.data_list).erase((d_manager.data_list).begin()+select);
        d_manager.down_data_cnt();
        
        
      break;

    case 4:
        temp.clear();
        searched_index.clear();
        memset(recv_buf, 0, sizeof(recv_buf));

        sprintf(buf, "%s", "검색하고 싶은 글의 제목을 입력하세요");
        send(sd, buf, strlen(buf), 0);
        n = recv(sd, recv_buf, sizeof(recv_buf), 0);
        temp = recv_buf;
        cout << "temp : " << temp << endl;
        for(int k=0; k<d_manager.get_data_cnt(); k++){
          if(temp == d_manager.get_data_title(k)){
            cout << "temp : " << temp << endl;
            cout << "data_title(k) : " << d_manager.get_data_title(k) << endl;
            searched_index.push_back(k);
            
          }
        }
        if(searched_index.size() == 0){
        sprintf(buf, "%s", "찾고자 하는 글이 없습니다. \n");
        send(sd, buf, strlen(buf), 0);
        sleep(3);
        }
        else if (searched_index.size() != 0){
          sprintf(buf, "%s %ld %s", "총 ",searched_index.size(), "개의 글을 찾았습니다. \n ");
          send(sd, buf, strlen(buf), 0);
          sleep(2); 
          sprintf(buf, "%s %ld %s", "총 ",searched_index.size(), "검색한 글만 보시겠습니까? [Y/n] \n ");
          send(sd, buf, strlen(buf), 0);
          memset(recv_buf, 0, sizeof(recv_buf));
          
          n = recv(sd, recv_buf, sizeof(recv_buf), 0);
          
          if (strcmp(recv_buf,"Y")==0){
            search_flag = 1;
          }
          

        }
        cout << "searched_index length : " << searched_index.size() << endl;
        //n = recv(sd, recv_buf, sizeof(recv_buf), 0);
        temp.clear();
      break;

    case 5:
      search_flag = 0;
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