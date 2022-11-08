// Client
#include <arpa/inet.h>
#include <error.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <pthread.h>
#include <cstring>
#include <string>
#include <string.h>
#include <fstream>
#include <termio.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <error.h>
#include <vector>
#include <time.h>
#include <algorithm>
#define CR '\012' //개행, 역슬레시 + n
#define MAX_DATA_SIZE 4028

using namespace std;
pthread_t tid1, tid2;
int check_flag = 0;
int upload_flag = 0;
int file_no = 0;

class myfile{
private:
  string file_name;
  int file_no;
  string file_pin;
  off_t file_size;
  string file_mtime;
public:
  myfile();
  myfile(string, int, string, off_t, string);
  ~myfile();

  string get_file_name();
  int get_file_no();
  string get_file_pin();
  off_t get_file_size();
  string get_file_time();
};

class file_Manager{
private:
  int file_cnt;
public:
  vector<myfile*> file_list;
  file_Manager();
  ~file_Manager();
  void add_file(myfile* myfile);
  int get_file_cnt();
  int get_file_postno(int i);
  string get_file_title(int i);
  string get_file_passwd(int i);
  off_t get_file_size_(int i);
  string get_file_time_(int i);
  void list_clear();  
};

myfile::myfile(){
  this->file_name = "";
  this->file_no = 999;
  this->file_pin = "";
  this->file_size = 999;
}

myfile::myfile(string file_name, int file_no, string file_pin, off_t file_size, string file_mtime){
  this->file_name = file_name;
  this->file_no = file_no;
  this->file_pin = file_pin;
  this->file_size = file_size;
  this->file_mtime = file_mtime;
}

myfile::~myfile(){

}

string myfile::get_file_name(){
  return this->file_name;
}

int myfile::get_file_no(){
  return this->file_no;
}

string myfile::get_file_pin(){
  return this->file_pin;
}

off_t myfile::get_file_size(){
  return this->file_size;
}

string myfile::get_file_time(){
  return this->file_mtime;
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

int file_Manager::get_file_cnt(){
  return file_cnt;
}

string file_Manager::get_file_time_(int i){
  return file_list[i]->get_file_time();
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

void file_Manager::list_clear(){
  file_list.clear();
  file_cnt = 0;
  file_no = 0;
}

bool cmp(myfile* a, myfile* b) { 
	return a->get_file_time() > b->get_file_time();
} 

file_Manager f_manager;
file_Manager f_manager_temp;
void *recv_thread(void *arg) {
  FILE* fp = NULL;
  string file_add;
  string temp;
  string temp_temp;
  char download_buf[1024];
  int new_fd = *((int *)arg);
  char recv_msg[MAX_DATA_SIZE];
  char printbuf[100];
  int n, a;
  char buf[1024];
  DIR *dir;
  struct dirent *diread;
  char ch;
  struct stat sb;
  struct tm* ts;
  string time_stamp;
  char printbuf2[100];

  while (1) {
    n = recv(new_fd, recv_msg, MAX_DATA_SIZE, 0);
    if (n<=0){
    cout << "Disconnected from server" << endl;
    break;
    }
    recv_msg[n] = '\0';
    if (strcmp(recv_msg, "WINDOW")==0){
      cout << endl;
      system("clear");
    }
    else if (strcmp(recv_msg, "LOGIN-") == 0){
      check_flag = 1;
      memset(recv_msg,0,sizeof(recv_msg));
      usleep(0.5);
    }
    else if (strcmp(recv_msg, "DOWNLOAD") == 0){ ///// 다운로드 준비 시작 /////
      cout << endl;
      memset(recv_msg,0,sizeof(recv_msg));
      n = recv(new_fd, recv_msg, MAX_DATA_SIZE, 0);
      temp.clear();
      temp = recv_msg;
      cout << " Down Load Original File Name : " << temp << endl;
      file_add = "/home/mobis/Public/Client/";
      file_add = file_add + "copy_" + temp;
      ofstream fdest(file_add, ios::out | ios::binary);
      memset(recv_msg,0,sizeof(recv_msg));
      cout << " New File name : " << file_add << endl;
      //while(1){          
      n = recv(new_fd, recv_msg, MAX_DATA_SIZE, 0);
      fdest.write(recv_msg,n);
      fdest.close();
      memset(recv_msg,0,sizeof(MAX_DATA_SIZE));
      cout << " Download Completed : " << file_add << endl;
      fflush(stdout);
  
    }
    else if (strcmp(recv_msg, "UPLOAD") == 0){ ///// 업로드 준비 시작 /////
      upload_flag = 1;
      f_manager_temp.list_clear();
      f_manager.list_clear();
      cout << endl;
      memset(recv_msg,0,sizeof(recv_msg));
      if ((dir = opendir("/home/mobis/Public/Client/")) != nullptr) {
        while ((diread = readdir(dir)) != nullptr) {
          // cout << "diread->d_name : " << diread->d_name << endl;
          if (strcmp(diread->d_name, ".") != 0) {
            if (strcmp(diread->d_name, "..") != 0) {
              string c_temp = "/home/mobis/Public/Client/";
              string t_temp = diread->d_name;
              c_temp = c_temp + t_temp;
              strcpy(buf,c_temp.c_str());
              //char* ctemp = strcat("/home/mobis/Public/Client/", diread->d_name);
              stat(buf, &sb);
              time_t now = sb.st_mtime;
              ts = localtime(&now);
              strftime(printbuf2, sizeof(printbuf2), "%Y-%m-%d %H:%M", ts);
              time_stamp = printbuf2;
              f_manager_temp.add_file(new myfile(diread->d_name, ++file_no, "1234", sb.st_size,time_stamp));
            }
          }
        }
        closedir(dir);
      } else {
        perror("opendir");
        return 0;
      }
    cout << "                                    C L I E N T     F O L D E R                              \n";
    cout << "=============================================================================================\n";
    cout << " [File ID.]        [FILE NAME]                      [Size in Byte]          [Date] \n";
    cout << "=============================================================================================\n";
    sort(f_manager_temp.file_list.begin(), f_manager_temp.file_list.end(), cmp);
    for (int p=0; p<f_manager_temp.get_file_cnt();p++){
      f_manager.add_file(new myfile(f_manager_temp.file_list[p]->get_file_name(), p+1,"1234",f_manager_temp.file_list[p]->get_file_size(),f_manager_temp.file_list[p]->get_file_time()));
    }



    for (int m = 0; m < f_manager.get_file_cnt(); m++) {
      memset(buf, 0, sizeof(buf));
      strcpy(printbuf,f_manager.get_file_title(m).c_str());
      strcpy(printbuf2,f_manager.get_file_time_(m).c_str());
      sprintf(buf, "      %-10d %-29s          %-15ld%-10s\n", f_manager.get_file_postno(m), printbuf, f_manager.get_file_size_(m),printbuf2);  
      cout << buf;
      usleep(0.5);            

      }
      cout << "=============================================================================================\n";
    }

    else {cout << recv_msg;}
    fflush(stdout);
  }
  pthread_cancel(tid2);
  pthread_cancel(tid1);
  pthread_exit(NULL);
}

void *send_thread(void *arg) {
  int new_fd = *((int *) arg);
  int n,a;
  string s;
  string temp_pin;
  struct termio tbuf, oldtbuf;
  char ch;
  struct dirent *diread;
  int select;
  struct stat sb;
  int file_no = 0;
  vector<char *> files;
  char buf[4048];
  char recv_msg[4048];
  string temp;
  string file_add;
  char send_msg[MAX_DATA_SIZE];
  while (1) {
    if(check_flag == 1)
    {
      memset(send_msg,0,sizeof(send_msg));
      
      if(ioctl(0,TCGETA, &tbuf)==-1){
        perror("ioctl");
        exit(1);
      }
      oldtbuf = tbuf;
      tbuf.c_lflag &= ~ICANON;
      tbuf.c_lflag &= ~ECHO;
      tbuf.c_cc[VMIN] = 1;
      tbuf.c_cc[VTIME] = 0;
      if(ioctl(0,TCSETAF,&tbuf)==-1){
        perror("ioctl");
        exit(1);
      }
      while(1){
        ch=getchar();
        if(ch==CR)
          break;
        temp_pin = temp_pin + ch;
        cout << "*";
      }
      if (ioctl(0,TCSETAF, &oldtbuf)==-1){
        perror("ioctl");
        exit(1);
      }
      temp_pin = temp_pin + '\0';
      temp_pin.copy(send_msg,temp_pin.length());
      n = strlen(send_msg);
      cout << "send msg : " << send_msg << endl;
      send(new_fd,send_msg,n,0);
      check_flag = 0;
    }
    else if(upload_flag == 1){ //클라이언트 폴더에서 서버로 전송.
      
      getline(cin, s);    // <--- 여기서 멈춰있네
      select = stoi(s);
      select = select - 1;
      temp = f_manager.get_file_title(select);
      strcpy(send_msg,temp.c_str());
      send(new_fd, send_msg, sizeof(send_msg), 0);
//      sleep(1);
      file_add = "/home/mobis/Public/Client/";
      file_add = file_add + temp;
      cout << "check point 1" << endl;
      ifstream fsrc(file_add, ios::in | ios::binary | ios::ate);
      int f_size = fsrc.tellg();
      cout << "f_size : " << f_size << endl;
  //    sleep(10);
      if (!fsrc) {
        cout << "open error" << endl;
        exit(1);
      }
      memset(send_msg, 0, sizeof(send_msg));
      cout << "check point 2" << endl;
      fsrc.read(send_msg, f_size);
      n = send(new_fd, send_msg, f_size, 0);
      cout << "send data : " << n << endl;
      sleep(4);
      fsrc.close();
      usleep(0.5);
      temp.clear();
      memset(buf, 0, sizeof(buf));
      upload_flag = 0;
      file_no = 0;
      f_manager.list_clear();
    }
    else {
    getline(cin, s);    // <--- 여기서 멈춰있네
    if(s == "quit"){
       break;}
    s = s+'\0';
    s.copy(send_msg,s.length());
    n = strlen(send_msg);
    send(new_fd, send_msg, n, 0);
    }
  }
  pthread_cancel(tid1);
  pthread_cancel(tid2);
  pthread_exit(NULL);
  
}

int main(int argc, char *argv[]) {
  int sockfd;
  socklen_t addr_len;
  struct sockaddr_in client_addr;
  struct sockaddr_in server_addr;
  struct hostent *he;
  he = gethostbyname(argv[1]);
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(60000);
  server_addr.sin_addr = *((struct in_addr *)he->h_addr);
  inet_ntoa(server_addr.sin_addr);
  memset(&(server_addr.sin_zero), '\0', 8);
  if (connect(sockfd, (struct sockaddr *)&server_addr,sizeof(struct sockaddr)) == -1){
    perror("connet");
    exit(1);
  }
  system("clear");
  cout << "Client Start-------" << endl;

  if (pthread_create(&tid1, NULL, recv_thread, &sockfd) != 0) {
    perror("pthread_create");
  }
  if (pthread_create(&tid2, NULL, send_thread, &sockfd) != 0) {
    perror("pthread_create");
  }
  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);
  close(sockfd);
  return 0;
}