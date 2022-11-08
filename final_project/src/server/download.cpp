#include "data.h"
#include "data_manager.h"
#include "file_manager.h"
#include "myfile.h"
#include "user.h"
#include "user_manager.h"
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <time.h>
#include <string.h>
#include <algorithm>
#define MAX_DATA_SIZE 4028
using namespace std;

bool cmp(myfile* a, myfile* b) { 
	return a->get_file_time() > b->get_file_time();
} 

int download(int sd, data_Manager &d_manager, file_Manager &f_manager, int &f_no) {
  char printbuf[100];
  char printbuf2[100];
  FILE *fp;
  int fd;
  DIR *dir;
  file_Manager f_manager_temp;
  char write_buf[2048];
  char buf[4028];
  struct dirent *diread;
  vector<char *> files;
  int end_flag = 0;
  string temp;
  string time_stamp;
  int a, n;
  int search_flag = 0;
  char recv_buf[4024];
  struct tm* ts;
  int file_no = 0;
  int select;
  char data_fp[1024];
  string addr = "/home/mobis/Public/Server/";
  string send_file;
  struct stat sb;
  vector<int> searched_index;
  ////////////////////////////////////////////// while 문 시작 /////////////////////////////////////////////
  while (end_flag != 1) {
    file_no = 0;
    f_manager.list_clear();
    f_manager_temp.list_clear();
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%s", "WINDOW");
    send(sd, buf, strlen(buf), 0);
    sleep(1);
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%s", "자료실 베타 테스트!\n\n");
    send(sd, buf, strlen(buf), 0);
    usleep(0.5);
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%s", "                                  S E R V E R    F O L D E R                        \n");
    send(sd, buf, strlen(buf), 0);
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%s", "=============================================================================================\n");
    send(sd, buf, strlen(buf), 0);
    
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%s"," [File ID.]        [FILE NAME]                      [Size in Byte]          [Date] \n");
    send(sd, buf, strlen(buf), 0);
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%s", "=============================================================================================\n");
    send(sd, buf, strlen(buf), 0);
    memset(buf, 0, sizeof(buf));
    usleep(0.5);

    //////////////////클라이언트에서 다운로드만
   
    if ((dir = opendir("/home/mobis/Public/Server/")) != nullptr) {
      while ((diread = readdir(dir)) != nullptr) {
        // cout << "diread->d_name : " << diread->d_name << endl;
        if (strcmp(diread->d_name, ".") != 0) {
          if (strcmp(diread->d_name, "..") != 0) {
            files.push_back(diread->d_name);
            string c_temp = "/home/mobis/Public/Server/";
            string t_temp = diread->d_name;
            c_temp = c_temp + t_temp;
            strcpy(buf,c_temp.c_str());
            stat(buf, &sb);
            time_t now = sb.st_mtime;
            ts = localtime(&now);
            strftime(printbuf2, sizeof(printbuf2), "%Y-%m-%d %H:%M", ts);
            time_stamp = printbuf2;
            f_manager_temp.add_file(new myfile(diread->d_name, ++file_no, "1234", sb.st_size, time_stamp));
            memset(printbuf2,0,sizeof(printbuf2));
          }
        }
      }
      closedir(dir);
    } else {
      perror("opendir");
      return EXIT_FAILURE;
    }

    
    sort(f_manager_temp.file_list.begin(), f_manager_temp.file_list.end(), cmp);
    
    for (int p=0; p<f_manager_temp.get_file_cnt();p++){
      f_manager.add_file(new myfile(f_manager_temp.file_list[p]->get_file_name(), p+1,"1234",f_manager_temp.file_list[p]->get_file_size(),f_manager_temp.file_list[p]->get_file_time()));
    }
    
    

    if (search_flag == 1) ///////////////////////// 검색 후 출력
    {
      memset(buf, 0, sizeof(buf));
      sprintf(buf, "%s", "현재창에는 검색하신 결과만 나타납니다. \n");
      send(sd, buf, strlen(buf), 0);
      for (int m = 0; m < searched_index.size(); m++) {
      memset(buf, 0, sizeof(buf));
      strcpy(printbuf,f_manager.get_file_title(searched_index[m]).c_str());
      strcpy(printbuf2,f_manager.get_file_time_(searched_index[m]).c_str());
      sprintf(buf, "      %-10d %-29s          %-15ld%-10s\n", f_manager.get_file_postno(searched_index[m]), printbuf, f_manager.get_file_size_(searched_index[m]),printbuf2);  
      send(sd, buf, strlen(buf), 0);
      usleep(0.5);
      }
    }   
    
    else if (search_flag == 0) ////////////////// 일반 출력
    {
      //sort(f_manager.file_list.begin(), f_manager.file_list.end(), cmp);
      for (int m = 0; m < f_manager.get_file_cnt(); m++) {
      memset(buf, 0, sizeof(buf));
      strcpy(printbuf,f_manager.get_file_title(m).c_str());
      strcpy(printbuf2,f_manager.get_file_time_(m).c_str());
      sprintf(buf, "      %-10d %-29s          %-15ld%-10s\n", f_manager.get_file_postno(m), printbuf, f_manager.get_file_size_(m), printbuf2);  
      send(sd, buf, strlen(buf), 0);
      usleep(0.5);
      }

    }
    

    sprintf(buf, "%s","=============================================================================================\n");
    send(sd, buf, strlen(buf), 0);
    usleep(0.5);
    sprintf(buf, "%s", " [0]. 새로고침\n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", " [1]. 파일다운로드/서버자료실 (from server to client) \n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", " [2]. 파일업로드/클라이언트자료실 (from client to server) \n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", " [3]. 파일삭제\n");
    send(sd, buf, strlen(buf), 0);
    if (search_flag != 1){
    sprintf(buf, "%s", " [4]. 파일검색\n");
    send(sd, buf, strlen(buf), 0);
    }
    if (search_flag == 1){
    sprintf(buf, "%s", " [5]. 검색모드해제◀\n");
    send(sd, buf, strlen(buf), 0);
    }
    sprintf(buf, "%s", " [9]. 나가기\n\n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", " Input >> ");
    send(sd, buf, strlen(buf), 0);
    memset(recv_buf,0,sizeof(recv_buf));
    n = recv(sd, recv_buf, sizeof(recv_buf), 0);
    // recv_buf[n] = '\0';
    a = atoi(recv_buf);
    memset(recv_buf, 0, sizeof(recv_buf));

    switch (a) {
    case 0:
      memset(buf, 0, sizeof(buf));
      sprintf(buf, "%s", "WINDOW");
      send(sd, buf, strlen(buf), 0);
      usleep(0.5);
      file_no = 0;
      break;

    case 1: //파일 다운로드
    {
      sprintf(buf, "%s", " 다운로드 원하시는 파일번호를 입력하세요 >> ");
      send(sd, buf, strlen(buf), 0);
      n = recv(sd, recv_buf, sizeof(recv_buf), 0);
      a = atoi(recv_buf); // 클라이언트로 부터 받아온 파일 번호..
      if (a > f_manager.get_file_cnt()){
        sprintf(buf, "%s", " Error :: 파일 ID를 재확인 해주세요. ");
        send(sd, buf, strlen(buf), 0);
        sleep(3);
        break;
      }
      else if (a <= 0){
        sprintf(buf, "%s", " Error :: 파일 ID를 재확인 해주세요. ");
        send(sd, buf, strlen(buf), 0);
        sleep(3);
        break;
      }
      else {
      for (int t = 0; t < f_manager.get_file_cnt(); t++) {
        if ((f_manager.get_file_postno(t)) == a) {
          select = t;
        }
      }
      memset(buf, 0, sizeof(buf));
      ////////// 클라이언트 다운로드 준비 시작 /////////////
      sprintf(buf, "%s", "DOWNLOAD");
      send(sd, buf, strlen(buf), 0);
      usleep(0.5);
      memset(buf, 0, sizeof(buf));
      /////////////////////////////// 1. 파일 이름 전달
      cout << "file name :" << f_manager.get_file_title(select) << endl;
      strcpy(buf, f_manager.get_file_title(select).c_str());
      cout << "buf : " << buf << endl;
      send(sd, buf, strlen(buf), 0);
      sleep(1);
      //////////////////////////////// 파일 이름 전달 종료
      temp = buf;
      addr = addr + temp;
      cout << "addr : " << addr << endl; // ex.) addr = "/home/mobis/Public/test3.txt";
      sleep(8);
      
      ifstream fsrc(addr, ios::in | ios::binary | ios::ate ); // 데이터를 보낼 파일을 오픈 한다.
      int f_size = fsrc.tellg();
      cout << "file size : " << f_size << endl;
      if (!fsrc) {
        cout << "open error" << endl;
      }
      memset(buf, 0, sizeof(buf));
      
      fsrc.read(buf, f_size);
      send(sd, buf, f_size, 0);
      sleep(2);
      fsrc.close();
      usleep(0.5);
      file_no = 0;
      temp.clear();
      memset(buf, 0, sizeof(buf));
      sprintf(buf, "%s", "WINDOW");
      send(sd, buf, strlen(buf), 0);
      usleep(0.5);
      }
    } break;
////////////////////////////////////////////////////////////////
    case 2: //파일 업로드
    {
      addr = "/home/mobis/Public/Server/";
      memset(buf,0,sizeof(buf));
      sprintf(buf, "%s", "UPLOAD"); //클라이언트에서 업로드 모드
      send(sd, buf, strlen(buf), 0);
      usleep(0.5);
      sleep(2);
      memset(buf, 0, sizeof(buf));
      sprintf(buf, "%s", " └ 내 자료실에 있는 파일을 확인하였나요? 계속진행? [Y/n] ");
      send(sd, buf, strlen(buf), 0);
      cout << "check p1" << endl;
      n = recv(sd, recv_buf, sizeof(recv_buf),0); //파일이름이 클라이언트에서 넘와야되는데..안넘어오네
      if (strcmp(recv_buf, "Y")!=0){
        break;
      }
      else{ ///////// 여기서 부터 client upload_flag 가 up
      temp.clear();
      memset(buf, 0, sizeof(buf));
      sprintf(buf, "%s", " └ 몇번 파일을 업로드 하시겠습니까? >> ");
      send(sd, buf, strlen(buf), 0);
      memset(recv_buf,0,sizeof(recv_buf));
      n = recv(sd, recv_buf, sizeof(recv_buf),0); /// upload flag에 getline
      cout << "check p2" << endl;
      cout << "recv : " << recv_buf << endl; //file name 
      temp = recv_buf; // temp 에 파일이름이 임시저장
      string f_name_temp = temp;
      strcpy(printbuf, temp.c_str());
      // file_add = "/home/mobis/Public/Client/";
      temp = addr + "cp_" + temp;
      ofstream fdest(temp, ios::out | ios::binary); //temp 파일을 연다.

      memset(recv_buf, 0, sizeof(recv_buf));
      cout << " New " << endl;
      n = read(sd, recv_buf, MAX_DATA_SIZE); //클라이언트에서 데이터를 받음.
      cout << "n : " << n << endl;
      fdest.write(recv_buf, n);
      fdest.close();

      temp.clear();
      memset(recv_buf, 0, sizeof(recv_buf));
      memset(buf, 0, sizeof(buf));
      sprintf(buf, " %s%s", printbuf, "을 서버자료실에 업로드 완료하였습니다. \n");
      send(sd, buf, strlen(buf), 0);
      f_name_temp = "cp_" + f_name_temp;
      strcpy(printbuf, f_name_temp.c_str());
      sprintf(buf, " %s%s\n\n", "▶ 새로운 파일 이름 : ", printbuf);
      send(sd, buf, strlen(buf), 0);
      sleep(8);
      }
    } 
    break;

    case 3: //파일 삭제
      temp.clear();
      memset(buf, 0, sizeof(buf));
      sprintf(buf, "%s", " └ 몇번 파일을 삭제 하시겠습니까? >> ");
      send(sd, buf, strlen(buf), 0);
      memset(recv_buf,0,sizeof(recv_buf));
      n = recv(sd, recv_buf, sizeof(recv_buf),0);
      a = atoi(recv_buf); // 클라이언트로 부터 받아온 파일 번호..
      if (a > f_manager.get_file_cnt()) {
        sprintf(buf, "%s", " Error :: 파일 ID를 재확인 해주세요. ");
        send(sd, buf, strlen(buf), 0);
        sleep(3);
        break;
      } else {
        memset(buf, 0, sizeof(buf));
        temp = temp + " └ 파일명 : " + f_manager.get_file_title(a - 1) + " 삭제하시겠습니까? [Y/n] >> ";
        strcpy(buf, temp.c_str());
        send(sd, buf, strlen(buf), 0);
        usleep(0.5);
        memset(recv_buf, 0, sizeof(recv_buf));
        n = recv(sd, recv_buf, sizeof(recv_buf), 0);
        if (strcmp(recv_buf, "Y") == 0) {
          f_manager.list_clear();
          temp = "rm -rf /home/mobis/Public/Server/" +
                 f_manager.get_file_title(a - 1);
          memset(printbuf, 0, sizeof(printbuf));
          strcpy(printbuf, temp.c_str());
          // execl("/bin/rm", "rm", printbuf, NULL);
          system(printbuf);
          memset(buf, 0, sizeof(buf));
          sprintf(buf, "%s", " Success :: 파일 삭제 성공! ");
          send(sd, buf, strlen(buf), 0);
          sleep(2);
          break;
        } else {
          memset(buf, 0, sizeof(buf));
          sprintf(buf, "%s", " Fail :: 파일 삭제 실패! ");
          send(sd, buf, strlen(buf), 0);
          sleep(2);
          break;
        }
        cout << "down check";
      }
      break;

    case 4: // 파일 검색
      temp.clear();
      searched_index.clear();
      memset(recv_buf, 0, sizeof(recv_buf));
      sprintf(buf, "%s", " └ 검색하고 싶은 자료의 키워드를 입력하세요. >> ");
      send(sd, buf, strlen(buf), 0);
      n = recv(sd, recv_buf, sizeof(recv_buf), 0);
      temp = recv_buf;
      //sort(f_manager.file_list.begin(), f_manager.file_list.end(), cmp);
      for(int k=0; k<f_manager.get_file_cnt(); k++){
        size_t found = f_manager.get_file_title(k).find(temp);
          if (found!=string::npos){
            searched_index.push_back(k);
          }
      }
      if(searched_index.size() == 0){
      sprintf(buf, "%s", " └ 찾고자 하는 키워드의 자료가 없습니다. \n");
      send(sd, buf, strlen(buf), 0);
      sleep(3);
      }
      else if (searched_index.size() != 0){
        sprintf(buf, "%s %ld %s", " 총 ",searched_index.size(), "개의 자료를 찾았습니다. \n");
        send(sd, buf, strlen(buf), 0); sleep(2);
        sprintf(buf, "%s %ld %s", " 총 ",searched_index.size(), "검색한 자료만보시겠습니까? [Y/n] >> ");
        send(sd, buf, strlen(buf), 0);
        memset(recv_buf, 0, sizeof(recv_buf));
        n = recv(sd, recv_buf, sizeof(recv_buf), 0);
        if (strcmp(recv_buf,"Y")==0){
          search_flag = 1;
        }
      }
      temp.clear();
      break;
    case 5:
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