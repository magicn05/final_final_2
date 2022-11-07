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
#define MAX_DATA_SIZE 2048

using namespace std;

pthread_t tid1, tid2;


void *recv_thread(void *arg) {
  FILE* fp = NULL;
  string file_add;
  string temp;
  string temp_temp;
  char download_buf[1024];
  int new_fd = *((int *)arg);
  char recv_msg[MAX_DATA_SIZE];
  int n;
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
    else if (strcmp(recv_msg, "DOWNLOAD") == 0){ ///// 다운로드 준비 시작 /////
      
      cout << endl;
      memset(recv_msg,0,sizeof(recv_msg));
      n = recv(new_fd, recv_msg, MAX_DATA_SIZE, 0);
      temp.clear();
      temp = recv_msg;
      cout << "temp : " << temp << endl;
      file_add = "/home/mobis/Public/Client/";
      file_add = file_add + temp + "_copy";
      
      ofstream fdest(file_add, ios::out | ios::binary);
      memset(recv_msg,0,sizeof(recv_msg));
      cout << "file name : " << file_add << endl;
      //while(1){          
      n = recv(new_fd, recv_msg, MAX_DATA_SIZE, 0);
      fdest.write(recv_msg,n);
      fdest.close();

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
  int n;
  string s;
  char send_msg[MAX_DATA_SIZE];
  while (1) {
    getline(cin, s);    
    if(s == "quit"){
       break;}
    s = s+'\0';
    s.copy(send_msg,s.length());
    n = strlen(send_msg);
    send(new_fd, send_msg, n, 0);
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