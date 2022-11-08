#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <string>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>
#include <signal.h>
#include "data.h"
#include "data_manager.h"
#include "file_manager.h"
#include "myfile.h"
#include "user.h"
#include "user_manager.h"
#include <fstream>
#include <pthread.h>
#include <stdlib.h>
#include <boost/archive/text_iarchive.hpp> 
#include <boost/archive/text_oarchive.hpp> 
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <fstream>
#include <iostream>

#define MAX_DATA_SIZE 1024
using namespace std;

pthread_mutex_t mutx;

int start_menu(int sd, user &now_user);
int board(int sd, data_Manager &d_manager, pthread_mutex_t &mutx, user& now_user);
int download(int sd, data_Manager &d_manager, file_Manager &f_manager,
             int &f_no);

int f_no = 0;

vector<int> client_no;

struct sockaddr_in clientaddr;

user_Manager u_manager;
data_Manager d_manager;
file_Manager f_manager;


class Server_Manager {
public:
  struct sockaddr_in server_addr;
  Server_Manager() {
    this->server_addr.sin_family = AF_INET;
    this->server_addr.sin_port = htons(60000);
    this->server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(&(this->server_addr.sin_zero), '\0', 8);
  }
};

void thread_function(int sd) {
  // load users
  user now_user;
  int n,a;
  char printbuf[100];
  int res = start_menu(sd, now_user); // 1번 , 2번의 선택..
  int end_flag = 0;
  char buf[1024];
  char recv_buf[1024];
  if (res == 0) // 0 번 : 연결종료
    close(sd);
  else if (res == 1) { // 1번 : 게시판
    //여기서 게시판 , 자료실, 채팅방 선택
    // download(sd, d_manager, f_manager, f_no);
    strcpy(printbuf,now_user.get_userid().c_str());
    while (end_flag != 1) {
      memset(buf, 0, sizeof(buf));
      sprintf(buf, "%s", "WINDOW");
      send(sd, buf, strlen(buf), 0);
      usleep(0.5);
      sprintf(buf, "%s%s", printbuf,"님 안녕하세요. 원하시는 메뉴를 선택해주세요 \n\n");
      send(sd, buf, strlen(buf), 0);
      memset(buf, 0, sizeof(buf));

      sprintf(buf, "%s", " [1]. 게시판▦ \n\n");
      send(sd, buf, strlen(buf), 0);
      memset(buf, 0, sizeof(buf));
      sprintf(buf, "%s", " [2]. 자료실♬\n\n");
      send(sd, buf, strlen(buf), 0);
      memset(buf, 0, sizeof(buf));
      sprintf(buf, "%s", " [3]. 채팅방♥\n\n");
      send(sd, buf, strlen(buf), 0);
      memset(buf, 0, sizeof(buf));
      sprintf(buf, "%s", " [4]. 로그아웃\n\n");
      send(sd, buf, strlen(buf), 0);
      memset(buf, 0, sizeof(buf));
      sprintf(buf, "%s", " Input >> ");
      send(sd, buf, strlen(buf), 0);
      memset(buf, 0, sizeof(buf));
      n = recv(sd, recv_buf, sizeof(recv_buf), 0);
      a = atoi(recv_buf);
      switch (a) {
      case 1: //게시판
        board(sd, d_manager, mutx, now_user);
        break;
      case 2: //자료실
        download(sd, d_manager, f_manager, f_no);
        break;
      case 3: //채팅방
      {
        string chat_name;
        string chat_msg;
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%s", "WINDOW");
        send(sd, buf, strlen(buf), 0);
        
        usleep(0.5);
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%s", " 모두의 채팅방에 오신것을 환영합니다. (나가기 : /exit 입력) \n\n");
        send(sd, buf, strlen(buf), 0);
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%s", " 대화에 사용할 이름을 입력하세요 >> ");
        usleep(0.5);
        send(sd, buf, strlen(buf), 0);
        memset(buf, 0, sizeof(buf));
        n = recv(sd, recv_buf, sizeof(recv_buf), 0);
        chat_name = recv_buf;
        memset(recv_buf, 0, sizeof(recv_buf));
        pthread_mutex_lock(&mutx);
        client_no.push_back(sd);
        pthread_mutex_unlock(&mutx);
        chat_msg = "[" + chat_name + "] 님이 채팅방에 접속하였습니다. " + '\n';
        strcpy(buf, chat_msg.c_str());
        pthread_mutex_lock(&mutx);
          for (int i = 0; i < client_no.size(); i++)
             send(client_no[i], buf, int(chat_msg.size()), 0);
            usleep(0.5);
        pthread_mutex_unlock(&mutx);
        memset(buf, 0, sizeof(buf));
        usleep(0.5);
        while ((n = recv(sd, recv_buf, sizeof(recv_buf), 0)) != 0) {
          if(strcmp(recv_buf, "/exit")!=0){
          chat_msg = chat_name;
          chat_msg = "[" + chat_msg + "] : " + recv_buf + '\n';
          memset(recv_buf,0, sizeof(recv_buf));
          strcpy(recv_buf, chat_msg.c_str());
          cout << "recv_buf : " << recv_buf << endl;
          pthread_mutex_lock(&mutx);
          for (int i = 0; i < client_no.size(); i++)
            send(client_no[i], recv_buf, int(chat_msg.size()), 0);
            usleep(0.5);
          pthread_mutex_unlock(&mutx);
          chat_msg.clear();
          memset(recv_buf,0,sizeof(recv_buf));}
          else if (strcmp(recv_buf, "/exit") ==0)
          {
            chat_msg = chat_name + "님이 나가셨습니다." + '\n';
            strcpy(recv_buf, chat_msg.c_str());
            pthread_mutex_lock(&mutx);
              for (int i = 0; i < client_no.size(); i++)
                send(client_no[i], recv_buf, int(chat_msg.size()), 0);
            usleep(0.5);
            int remove_sd;
            for (int v=0; v<client_no.size(); v++){
              if (client_no[v] == sd){
                remove_sd = v;
              }
            }
            client_no.erase(client_no.begin() + remove_sd);
            //채팅방에서 나가면 나갔다고 전달하고, client vector에서 제거.
            pthread_mutex_unlock(&mutx);
            break;
          }
        }
        
      } break;

      case 4: //로그아웃
        end_flag = 1;
        break;
      }
    }

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%s", "WINDOW");
    send(sd, buf, strlen(buf), 0);
    usleep(0.5);
    sprintf(buf, "%s ", "접속이 끊어집니다. 오늘도 좋은 하루 보내세요. ~ \n\n");
    send(sd, buf, strlen(buf), 0);
    sleep(2);
    close(sd);
  }
  return;
}

void handler(int signo){
  cout << endl;
  cout << "S E R V E R              D O W N (in 3 secs)" <<endl;
  ofstream user_ofs("userinfo.txt");
  {
    boost::archive::text_oarchive user_oa(user_ofs);
    user_oa << u_manager;
  }

  ofstream board_ofs("board_contents.txt");
  {
    boost::archive::text_oarchive board_oa(board_ofs);
    board_oa << d_manager;
  }
  cout << "good bye" << endl;
  //sleep(1);
  //cout << 2 << endl;
  //sleep(1);
  //cout << 1 << endl;
  //sleep(1);
  // dlclose(handle1);
  // dlclose(handle2);
  // dlclose(handle3);
  exit(1);
}

int main() {
  // handle1 = dlopen ("/home/mobis/final_final_2/final_project/lib/libmanager.so", RTLD_NOW);
  // handle2 = dlopen ("/home/mobis/final_final_2/final_project/lib/libdownload.so", RTLD_NOW);
  // handle3 = dlopen ("/home/mobis/final_final_2/final_project/lib/libboard.so", RTLD_NOW);

  cout << "S E R V E R          O N" << endl;
  signal(SIGINT, handler);
  pthread_mutex_init(&mutx, NULL);
  data a;
 
  {
        // create and open an archive for input
        std::ifstream user_ifs("userinfo.txt");
        
        boost::archive::text_iarchive user_ia(user_ifs);
        // read class state from archive
        user_ia >> u_manager;
        //ia_temp >> newg_temp;
        // archive and stream closed when destructors are called
  }

  {
        // create and open an archive for input
        std::ifstream board_ifs("board_contents.txt");
        
        boost::archive::text_iarchive board_ia(board_ifs);
        // read class state from archive
        board_ia >> d_manager;
        //ia_temp >> newg_temp;
        // archive and stream closed when destructors are called
  }

  int choice;
  int sockfd, new_fd, state;
  socklen_t client_len;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  Server_Manager Server_Set;
  int reuseAddress = 1;
  int option = 1;

  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

  if (bind(sockfd, (struct sockaddr *)&(Server_Set.server_addr),
           sizeof(struct sockaddr)) == -1) {
    perror("bind");
    exit(1);
  }

  state = listen(sockfd, 5);
  if (state == -1) {
    perror("listen error : ");
    exit(0);
  }
  while (1) {

    new_fd = accept(sockfd, (struct sockaddr *)&clientaddr, &client_len);

    thread t1(thread_function, new_fd);
    t1.detach();
  }
  close(sockfd);
}

int start_menu(int sd, user &now_user) {
  string id, name, pin, phone_num;

  char buf[1024];
  char recv_buf[1024];
  int n, a, b;
  int end_flag = 0;
  int out_flag = 0;

  while (end_flag != 1) {
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%s", "WINDOW");
    send(sd, buf, strlen(buf), 0);
    sleep(1);
    sprintf(buf, "%s", "S T A R T --- M E N U \n\n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", "메뉴를 선택하세요\n\n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", " [1]. 로그인\n\n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", " [2]. 회원 가입\n\n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", " [3]. 연결 끊기\n\n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", " Input >> ");
    send(sd, buf, strlen(buf), 0);
    n = recv(sd, buf, sizeof(buf), 0);
    buf[n] = '\0';
    a = atoi(buf);

    switch (a) {
    case 1:
      sprintf(buf, "%s", "WINDOW");
      send(sd, buf, strlen(buf), 0);
      sleep(2);
      sprintf(buf, "%s", "로그인 페이지 입니다. \n\n");
      send(sd, buf, strlen(buf), 0);
      sleep(2);
      sprintf(buf, "%s", "ID  >> ");
      send(sd, buf, strlen(buf), 0);
      memset(buf,0,sizeof(buf));
      sleep(2);
      memset(recv_buf, 0, sizeof(recv_buf));
      sprintf(buf, "%s", "LOGIN-");
      send(sd, buf, strlen(buf), 0);
      sleep(1); // delay 여기?????      
      n = recv(sd, recv_buf, sizeof(recv_buf), 0);
      usleep(0.5);
      id = recv_buf;
      memset(recv_buf, 0, sizeof(recv_buf));
      usleep(0.5);
      
      sprintf(buf, "%s", "PIN >> ");
      send(sd, buf, strlen(buf), 0);
      usleep(0.5);

      n = recv(sd, recv_buf, sizeof(recv_buf), 0);
      pin = recv_buf;
      cout << "recv_buf : " << recv_buf << endl;
      usleep(0.5);
      memset(recv_buf, 0, sizeof(recv_buf));
      cout << "id" << id << endl;
      cout << "pin" << pin << endl;
      if (u_manager.check_login(id, pin) == 0) {
        sprintf(buf, "%s", "Log-in Succeed \n\n");
        send(sd, buf, strlen(buf), 0);
        usleep(0.5);
        now_user.set_userid(id);
        now_user.set_userpin(pin);
        end_flag = 1;
        out_flag = 1;

      } else {
        sprintf(buf, "%s", "Impossible to login \n\n");
        send(sd, buf, strlen(buf), 0);
        usleep(0.5);
      }

      memset(buf, 0, sizeof(buf));
      break;

    case 2:
      sprintf(buf, "%s", "WINDOW");
      send(sd, buf, strlen(buf), 0);
      sleep(1);
      sprintf(buf, "%s", "회원가입 페이지 입니다. \n\n");
      send(sd, buf, strlen(buf), 0);

      sprintf(buf, "%s", "New ID  >> ");
      send(sd, buf, strlen(buf), 0);

      n = recv(sd, recv_buf, sizeof(recv_buf), 0);
      id = recv_buf;
      memset(recv_buf, 0, sizeof(recv_buf));

      sprintf(buf, "%s", "New PIN >> ");
      send(sd, buf, strlen(buf), 0);

      n = recv(sd, recv_buf, sizeof(recv_buf), 0);
      pin = recv_buf;
      memset(recv_buf, 0, sizeof(recv_buf));

      sprintf(buf, "%s", "User Name >> ");
      send(sd, buf, strlen(buf), 0);
      n = recv(sd, recv_buf, sizeof(recv_buf), 0);
      name = recv_buf;
      memset(recv_buf, 0, sizeof(recv_buf));

      sprintf(buf, "%s", "Phone_number >> ");
      send(sd, buf, strlen(buf), 0);
      n = recv(sd, recv_buf, sizeof(recv_buf), 0);
      phone_num = recv_buf;

      u_manager.add_user(new user(id, pin, name, phone_num));

      sprintf(buf, "%s", "성공적으로 생성되었습니다. 잠시대기");
      send(sd, buf, strlen(buf), 0);
      sleep(1);

      memset(buf, 0, sizeof(buf));
      memset(recv_buf, 0, sizeof(recv_buf));
      break;

    case 3:
      sprintf(buf, "%s", "");
      send(sd, buf, 0, 0);
      end_flag = 1;
      break;

    case 4:

      for (int i = 0; i < u_manager.get_user_cnt(); i++) {
        u_manager.get_user_id(i);
        u_manager.get_user_pin(i);
        u_manager.get_user_name(i);
        u_manager.get_user_phoneNum(i);
        cout << endl;
      }

      break;
    } // switch end
  }   // while end

  if (out_flag == 1)
    return 1;
  else
    return 0;
}
