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
#include <fstream>
#include <termio.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <error.h>
#include <vector>
#define CR '\012' //개행, 역슬레시 + n
#define MAX_DATA_SIZE 2048

using namespace std;
pthread_t tid1, tid2;
int check_flag = 0;
int upload_flag = 0;
class myfile{
private:
  string file_name;
  int file_no;
  string file_pin;
  long long int file_size;
public:
  myfile();
  myfile(string, int, string, long long int);
  ~myfile();

  string get_file_name();
  int get_file_no();
  string get_file_pin();
  long long int get_file_size();
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
  int get_file_size_(int i);
  void list_clear();
  
};

myfile::myfile(){
  this->file_name = "";
  this->file_no = 999;
  this->file_pin = "";
  this->file_size = 999;
}

myfile::myfile(string file_name, int file_no, string file_pin, long long int file_size){
  this->file_name = file_name;
  this->file_no = file_no;
  this->file_pin = file_pin;
  this->file_size = file_size;
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

long long int myfile::get_file_size(){
  return this->file_size;
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

int file_Manager::get_file_postno(int i){
  return file_list[i]->get_file_no();
}

string file_Manager::get_file_title(int i){
  return file_list[i]->get_file_name();
}

string file_Manager::get_file_passwd(int i){
  return file_list[i]->get_file_pin();
}

int file_Manager::get_file_size_(int i){
  return file_list[i]->get_file_size();
}

void file_Manager::list_clear(){
  file_list.clear();
  file_cnt = 0;
  //f_no = 0;


}

file_Manager f_manager;


void *recv_thread(void *arg) {
  FILE* fp = NULL;
  string file_add;
  string temp;
  string temp_temp;
  char download_buf[1024];
  int new_fd = *((int *)arg);
  char recv_msg[MAX_DATA_SIZE];
  vector<char *> files;
  int n, a;
  char buf[1024];
  DIR *dir;
  struct dirent *diread;
  struct termio tbuf, oldtbuf;
  char ch;
  struct stat sb;
  int file_no = 0;
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
    else if (strcmp(recv_msg, "LOGIN") == 0){
      check_flag = 1;
      
    }
    else if (strcmp(recv_msg, "DOWNLOAD") == 0){ ///// 다운로드 준비 시작 /////
      cout << endl;
      memset(recv_msg,0,sizeof(recv_msg));
      n = recv(new_fd, recv_msg, MAX_DATA_SIZE, 0);
      temp.clear();
      temp = recv_msg;
      cout << "Down Load Original File Anme : " << temp << endl;
      file_add = "/home/mobis/Public/Client/";
      file_add = file_add + "copy_" + temp;
      
      ofstream fdest(file_add, ios::out | ios::binary);
      memset(recv_msg,0,sizeof(recv_msg));
      cout << "New File name : " << file_add << endl;
      
      //while(1){          
      n = recv(new_fd, recv_msg, MAX_DATA_SIZE, 0);
      fdest.write(recv_msg,n);
      fdest.close();
      memset(recv_msg,0,sizeof(MAX_DATA_SIZE));
      cout << "Download Completed : " << file_add << endl;
      fflush(stdout);
  
    }

    else if (strcmp(recv_msg, "UPLOAD") == 0){ ///// 업로드 준비 시작 ///// + 내 폴더에 있는 자료 읽어오기.
      upload_flag = 1;
      cout << endl;
      memset(recv_msg,0,sizeof(recv_msg));

      if ((dir = opendir("/home/mobis/Public/Client/")) != nullptr) {
        while ((diread = readdir(dir)) != nullptr) {
          // cout << "diread->d_name : " << diread->d_name << endl;
          if (strcmp(diread->d_name, ".") != 0) {
            if (strcmp(diread->d_name, "..") != 0) {
              files.push_back(diread->d_name);
              stat(diread->d_name, &sb);
              f_manager.add_file(
                  new myfile(diread->d_name, ++file_no, "1234", sb.st_size));
            }
          }
        }
        closedir(dir);
      } else {
        perror("opendir");
        return 0;
      }
    cout << "================== C L I E N T     F O L D E R =====================\n";
    cout << " [File No.]                 [FILE NAME]                    [Size]  \n";
    cout << "===================================================================\n";

    for (int m = 0; m < f_manager.get_file_cnt(); m++) {
      temp.clear();
      temp = temp + "   ";
      temp = temp + to_string(f_manager.get_file_postno(m));
      temp = temp + "                       ";
      temp = temp + f_manager.get_file_title(m);
      temp = temp + "                   ";
      temp = temp + to_string(f_manager.get_file_size_(m));
      cout << temp << endl;
      //sprintf(buf, "%-10d          %-25s          %-10d", f_manager.get_file_postno(m), f_manager.get_file_title(m), f_manager.get_file_size_(m));
      temp.clear(); }
      
      cout << "몇번 파일을 업로드 하시겠습니까? >> ";
    //   cout << f_manager.get_file_title(2) << endl;
    //   cin >> a;
      
    //   cout << "선택한 파일 이름 : " << f_manager.get_file_title(a) << endl;
    //   temp = f_manager.get_file_title(a);
    //   strcpy(recv_msg,temp.c_str());
    //   //send(new_fd, recv_msg, MAX_DATA_SIZE,0); //파일 제목을 넘겨줌.
    //   file_add = "/home/mobis/Public/Client/";
    //   file_add = file_add + temp;
    //   cout << "file full name : " << file_add << endl;

    //   ifstream fsrc(file_add, ios::in | ios::binary);
    //   if (!fsrc) {
    //     cout << "open error" << endl;
    //   }
    //   memset(buf, 0, sizeof(buf));
    //   fsrc.read(buf, 1024);
    //   send(new_fd, buf, 1024, 0);
    //   sleep(2);
    //   fsrc.close();
    //   usleep(0.5);
    //   file_no = 0;
    //   temp.clear();
    //   memset(buf, 0, sizeof(buf));
       
  
    }

    else
      cout << recv_msg;
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
  
  struct stat sb;
  int file_no = 0;
  vector<char *> files;
  char buf[1024];
  char recv_msg[1024];
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
    else if(upload_flag == 1){
      cin >> a;
      cout << f_manager.get_file_title(a) << endl;
    
      
      cout << "선택한 파일 이름 : " << f_manager.get_file_title(a) << endl;
      temp = f_manager.get_file_title(a);
      strcpy(recv_msg,temp.c_str());
      //send(new_fd, recv_msg, MAX_DATA_SIZE,0); //파일 제목을 넘겨줌.
      file_add = "/home/mobis/Public/Client/";
      file_add = file_add + temp;
      cout << "file full name : " << file_add << endl;

      ifstream fsrc(file_add, ios::in | ios::binary);
      if (!fsrc) {
        cout << "open error" << endl;
      }
      memset(buf, 0, sizeof(buf));
      fsrc.read(buf, 1024);
      send(new_fd, buf, 1024, 0);
      sleep(2);
      fsrc.close();
      usleep(0.5);
      file_no = 0;
      temp.clear();
      memset(buf, 0, sizeof(buf));

    }
    else {
    getline(cin, s);    
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