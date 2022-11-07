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
using namespace std;

int download(int sd, data_Manager &d_manager, file_Manager &f_manager, int &f_no) {
  FILE *fp;
  int fd;
  DIR *dir;
  char write_buf[2048];
  char buf[1024];
  struct dirent *diread;
  vector<char *> files;
  int end_flag = 0;
  string temp;
  int a, n;
  int search_flag = 0;
  char recv_buf[1024];
  int file_no = 0;
  int select;
  char data_fp[1024];
  string addr = "/home/mobis/Public/Server/";
  string send_file;
  struct stat sb;

  while (end_flag != 1) {
    f_manager.list_clear();
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%s", "WINDOW");
    send(sd, buf, strlen(buf), 0);
    sleep(1);
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%s", "자료실 베타 테스트!\n\n");
    send(sd, buf, strlen(buf), 0);
    usleep(0.5);
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%s",
            "=================================================================="
            "==========\n");
    send(sd, buf, strlen(buf), 0);
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%s",
            " [File No.]                 [FILE NAME]                    [Size] "
            "       \n");
    send(sd, buf, strlen(buf), 0);
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%s",
            "=================================================================="
            "==========\n");
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
            stat(diread->d_name, &sb);
            f_manager.add_file(new myfile(diread->d_name, ++file_no, "1234", sb.st_size));
          }
        }
      }
      closedir(dir);
    } else {
      perror("opendir");
      return EXIT_FAILURE;
    }

    // for (auto file : files){
    //   cout << file << endl;
    // }

    ////////////////////////////////////////////////////////////////////////////////////////
    
    for (int m = 0; m < f_manager.get_file_cnt(); m++) {
      memset(buf, 0, sizeof(buf));
      
      temp.clear();
      temp = temp + "   ";
      temp = temp + to_string(f_manager.get_file_postno(m));
      temp = temp + "                       ";
      temp = temp + f_manager.get_file_title(m);
      temp = temp + "                   ";
      temp = temp + to_string(f_manager.get_file_size_(m));
      strcpy(buf, temp.c_str());
      buf[temp.size()] = '\n';
      //sprintf(buf, "%-10d          %-25s          %-10d", f_manager.get_file_postno(m), f_manager.get_file_title(m), f_manager.get_file_size_(m));
      send(sd, buf, strlen(buf), 0);
      usleep(0.5);
    }
    sprintf(buf, "%s",
            "==================== S E R V E R    F O L D E R =====================\n");
    send(sd, buf, strlen(buf), 0);
    usleep(0.5);
    sprintf(buf, "%s", "[0]. 새로고침\n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", "[1]. 파일다운로드\n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", "[2]. 파일업로드\n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", "[3]. 파일삭제\n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", "[4]. 파일검색\n");
    send(sd, buf, strlen(buf), 0);
    sprintf(buf, "%s", "[9]. 나가기\n");
    send(sd, buf, strlen(buf), 0);

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
      sprintf(buf, "%s", "다운로드 원하시는 파일번호를 입력하세요 >> ");
      send(sd, buf, strlen(buf), 0);
      n = recv(sd, recv_buf, sizeof(recv_buf), 0);
      a = atoi(recv_buf); // 클라이언트로 부터 받아온 파일 번호..

      for (int t = 0; t < f_manager.get_file_cnt(); t++) {
        if ((f_manager.get_file_postno(t)) == a) {
          select = t;
        }
      }
      // cout << "선택한 파일 이름 : " << f_manager.get_file_title(select) <<
      // endl;
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
      cout << "addr : " << addr << endl;
      // addr = "/home/mobis/Public/test3.txt";

      ifstream fsrc(addr, ios::in | ios::binary);
      if (!fsrc) {
        cout << "open error" << endl;
      }
      memset(buf, 0, sizeof(buf));
      fsrc.read(buf, 1024);
      send(sd, buf, 1024, 0);
      sleep(2);
      fsrc.close();
      usleep(0.5);
      file_no = 0;
      temp.clear();
      memset(buf, 0, sizeof(buf));
      sprintf(buf, "%s", "WINDOW");
      send(sd, buf, strlen(buf), 0);
      usleep(0.5);
    } break;

    case 2: //파일 업로드
      {sprintf(buf, "%s", "UPLOAD");
      send(sd, buf, strlen(buf), 0);
      usleep(0.5);
      //클라이언트에서 업로드 모드 
      memset(buf, 0, sizeof(buf));
      
      cout << "check p1" << endl;
      n = recv(sd, recv_buf, sizeof(recv_buf), 0); //파일이름이 클라이언트에서 넘어옴
      temp.clear();
      cout << "check p2" << endl;
      temp = recv_buf; //temp 에 파일이름이 임시저장
      cout << "temp : " << temp << endl;
      //file_add = "/home/mobis/Public/Client/";
      temp = addr + "_copy_from_client_" + temp;

      ofstream fdest(temp, ios::out | ios::binary);
      memset(recv_buf,0,sizeof(recv_buf));
            
      //n = recv(sd, recv_buf, 1024, 0);
      fdest.write(recv_buf,n);
      fdest.close();
      memset(recv_buf,0,sizeof(1024));
      }
      break;

    case 3: //파일 삭제

      break;

    case 4: // 파일 검색
            // temp.clear();
            // searched_index.clear();
            // memset(recv_buf, 0, sizeof(recv_buf));

      // sprintf(buf, "%s", "검색하고 싶은 글의 제목을 입력하세요");
      // send(sd, buf, strlen(buf), 0);
      // n = recv(sd, recv_buf, sizeof(recv_buf), 0);
      // temp = recv_buf;
      // cout << "temp : " << temp << endl;
      // for(int k=0; k<d_manager.get_data_cnt(); k++){
      //   if(temp == d_manager.get_data_title(k)){
      //     cout << "temp : " << temp << endl;
      //     cout << "data_title(k) : " << d_manager.get_data_title(k) << endl;
      //     searched_index.push_back(k);

      //   }
      // }
      // if(searched_index.size() == 0){
      // sprintf(buf, "%s", "찾고자 하는 글이 없습니다. \n");
      // send(sd, buf, strlen(buf), 0);
      // sleep(3);
      // }
      // else if (searched_index.size() != 0){
      //   sprintf(buf, "%s %ld %s", "총 ",searched_index.size(), "개의 글을
      //   찾았습니다. \n "); send(sd, buf, strlen(buf), 0); sleep(2);
      //   sprintf(buf, "%s %ld %s", "총 ",searched_index.size(), "검색한 글만
      //   보시겠습니까? [Y/n] \n "); send(sd, buf, strlen(buf), 0);
      //   memset(recv_buf, 0, sizeof(recv_buf));

      //   n = recv(sd, recv_buf, sizeof(recv_buf), 0);

      //   if (strcmp(recv_buf,"Y")==0){
      //     search_flag = 1;
      //   }

      // }
      // cout << "searched_index length : " << searched_index.size() << endl;
      // //n = recv(sd, recv_buf, sizeof(recv_buf), 0);
      // temp.clear();
      break;

    case 9:
      end_flag = 1;
      break;
    }
    usleep(0.5);
    memset(recv_buf, 0, sizeof(recv_buf));
    
  }

  // string add = "/home/mobis/Public/";
  // add = add + files[0];
  // // cout << "add : " << add << endl;
  // strcpy(buf, add.c_str());
  // fp = fopen(buf, "r");

  // fread(write_buf, sizeof(char), sizeof(write_buf),
  //       fp); // file stream 에 있는 걸 write_buf로 읽어 옵니다.

  // temp = write_buf;

  // cout << "temp: " << temp << endl;

  // cout << " ============= " << endl;
  //fclose(fp);
  // usleep(0.5);

  // cout << files[0] << endl;
  // cout << files[1] << endl;
  // cout << files[2] << endl;
  // cout << files[3] << endl;
  return 0;
}

// int main() {
//     DIR *dir; struct dirent *diread;
//     vector<char *> files;

//     if ((dir = opendir("/")) != nullptr) {
//         while ((diread = readdir(dir)) != nullptr) {
//             files.push_back(diread->d_name);
//         }
//         closedir (dir);
//     } else {
//         perror ("opendir");
//         return EXIT_FAILURE;
//     }

//     for (auto file : files) cout << file << "| ";
//     cout << endl;

//     return EXIT_SUCCESS;
// }

// int main(int argc, char**argv)
// {
//     if(argc != 2)
//     {
//         fprintf(stderr, "Usage : %s directory\n", argv[0]);
//         return 1;
//     }
//     struct stat hmm;

//     if (lstat(argv[1], &hmm) == -1){
//         perror("lstat");
//         return 0;
//     }

//     if(S_ISDIR(hmm.st_mode)){
//         DIR* dp = opendir(argv[1]);
//         if(dp==NULL){
//             perror("opendir");
//             return 1;
//         }
//         struct dirent* direntp;

//         while(1){
//             direntp = readdir(dp);
//             if(direntp == NULL)
//             {
//                 break;
//             }
//             printf("inode : %ld, name : %s\n", direntp->d_ino,
//             direntp->d_name);
//         }
//     }
//     return 0;
// }