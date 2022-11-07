// #include <iostream>
// #include <string>
// #include <vector>
// #include <fstream>

// using namespace std;

// int main()
// {
//   string addr = "/home/mobis/Public/";
//   string send_file = addr + "test3.txt";
//   string recv_file = addr + "test3.txt" + "_copy";


//   cout << send_file << endl;
//   cout << recv_file << endl;

  
//   ifstream fsrc(send_file, ios::in | ios::binary);
//   ofstream fdest(recv_file, ios::out | ios::binary);
//   char buf[1024];
//   while(!fsrc.eof()){
//     fsrc.read(buf, 1024);
//     int n = fsrc.gcount();
//     fdest.write(buf,n);
//   }  

//   fsrc.close();
//   fdest.close(); 


// }
 /// server 
#include <stdio.h> //입출력
#include <stdlib.h> //문자열 변환, 의사 난수 생성
#include <unistd.h> //표준 심볼 상수 및 자료형
#include <string.h> //문자열 상수
#include <arpa/inet.h> //주소변환
#include <sys/socket.h> //소켓 연결
#include <netinet/in.h> //IPv4 전용 기능
#include <pthread.h> //쓰레드 사용

#define BUF_SIZE 100
#define MAX_CLNT 256

void * handle_clnt(void * arg); //클라이언트로 부터 받은 메시지를 처리
void send_msg(char * msg, int len); //클라이언트로 부터 메시지를 받아옴
void error_handling(char * msg); //에러 처리

int clnt_cnt=0; //클라이언트 수 카운팅
int clnt_socks[MAX_CLNT]; //최대 클라이언트 소켓
pthread_mutex_t mutx; //mutex : 쓰레드 동시접근 해결

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock; //서버소켓, 클라이언트 소켓 변수 선언
	struct sockaddr_in serv_adr, clnt_adr; //서버,클라이언트 주소 변수 선언
	int clnt_adr_sz; //클라이언트 주소 크기
	pthread_t t_id; //쓰레드 아이디
	if(argc!=2) { //prot번호 받기
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
  
	pthread_mutex_init(&mutx, NULL); //mutex 초기화
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);  //서버 소켓 연결

	memset(&serv_adr, 0, sizeof(serv_adr)); //초기화
	serv_adr.sin_family=AF_INET; 
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1) //bind
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1) //listen
		error_handling("listen() error");
	
	while(1) //값 받아오기
	{
		clnt_adr_sz=sizeof(clnt_adr); //크기 할당
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz); //소켓 할당
		
		pthread_mutex_lock(&mutx); //쓰레드 잡기
		clnt_socks[clnt_cnt++]=clnt_sock; //클라이언트 소켓 받기
		pthread_mutex_unlock(&mutx); // 쓰레드 놓아줌
	
		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock); //쓰레드 생성, handle_clnt함수 실행
		pthread_detach(t_id); //쓰레드 소멸
		printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
	}
	close(serv_sock);
	return 0;
}
	
void * handle_clnt(void * arg) //클라이언트가 보낸 메시지를 핸들링
{
	int clnt_sock=*((int*)arg); //클라이언트 소켓 할당
	int str_len=0, i; //문자열 길이 선언
	char msg[BUF_SIZE]; //메시지 크기 설정(100)
	
	while((str_len=read(clnt_sock, msg, sizeof(msg)))!=0) //클라이언트가 보낸 메시지를 읽음
  //자신이 ㅏ담당하는 클라이언트가 매세지를 보낼경우, 서버는 그 메세지를 다른 클라이언트에 보내야됨.
		send_msg(msg, str_len); // 메시지 읽는 함수실행
	
	pthread_mutex_lock(&mutx); //먼저 생선된 쓰레드를 보호
	for(i=0; i<clnt_cnt; i++)   // remove disconnected client
	{
		if(clnt_sock==clnt_socks[i])
		{
			while(i++<clnt_cnt-1)
				clnt_socks[i]=clnt_socks[i+1];
			break;
		}
	}
	clnt_cnt--;
	pthread_mutex_unlock(&mutx); //쓰레드를 보호에서 해제
	close(clnt_sock);
	return NULL;
}

void send_msg(char * msg, int len) //mutex로 잡고있는 모든 내용을 보냄
{
	int i;
	pthread_mutex_lock(&mutx);
	for(i=0; i<clnt_cnt; i++)
		write(clnt_socks[i], msg, len);
	pthread_mutex_unlock(&mutx);
}

void error_handling(char * msg) //에러 핸들링 코드
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}