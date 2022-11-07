//header file
#include <stdio.h> //입출력
#include <stdlib.h> //문자열 변환, 의사 난수 생성
#include <unistd.h> //표준 심볼 상수 및 자료형
#include <string.h> //문자열 상수
#include <arpa/inet.h> //주소변환
#include <sys/socket.h> //소켓 연결
#include <pthread.h> //쓰레드 사용
	
#define BUF_SIZE 100 // 버퍼 사이즈
#define NAME_SIZE 20 //사용자 이름
	
void * send_msg(void * arg); //메시지를 보냄
void * recv_msg(void * arg); // 메시지를 받음
void error_handling(char * msg); // 에러 핸들링
	
char name[NAME_SIZE]="[DEFAULT]"; //이름 선언 및 초기화
char msg[BUF_SIZE]; // 메시지 선언
	
int main(int argc, char *argv[])
{
	int sock; // 소켓 선언
	struct sockaddr_in serv_addr; //소켓 주소 선언
	pthread_t snd_thread, rcv_thread; //보내고 받는 쓰레드 선언
	void * thread_return; //ip, port, 이름 받기
	if(argc!=4) {
		printf("Usage : %s <IP> <port> <name>\n", argv[0]);
		exit(1);
	 }
	
	sprintf(name, "[%s]", argv[3]);// 이름 입력 받음
	sock=socket(PF_INET, SOCK_STREAM, 0); //서버 소켓 연결
	
	memset(&serv_addr, 0, sizeof(serv_addr)); //초기화
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));
	  
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1) //연결
		error_handling("connect() error");
	
	pthread_create(&snd_thread, NULL, send_msg, (void*)&sock); //메세지 보냄
	pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock); // 메세지 받음
	pthread_join(snd_thread, &thread_return); // send 쓰레드 소멸
	pthread_join(rcv_thread, &thread_return); // recv 쓰레드 소멸
	close(sock);  
	return 0;
}
	
void * send_msg(void * arg)   // send thread main
{
	int sock=*((int*)arg);
	char name_msg[NAME_SIZE+BUF_SIZE];
	while(1) 
	{
		fgets(msg, BUF_SIZE, stdin);
		if(!strcmp(msg,"q\n")||!strcmp(msg,"Q\n")) 
		{
			close(sock);
			exit(0);
		}
		sprintf(name_msg,"%s %s", name, msg);
		write(sock, name_msg, strlen(name_msg));
	}
	return NULL;
}
	
void * recv_msg(void * arg)   // read thread main
{
	int sock=*((int*)arg);
	char name_msg[NAME_SIZE+BUF_SIZE];
	int str_len;
	while(1)
	{
		str_len=read(sock, name_msg, NAME_SIZE+BUF_SIZE-1);
		if(str_len==-1) 
			return (void*)-1;
		name_msg[str_len]=0;
		fputs(name_msg, stdout);
	}
	return NULL;
}
	
void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}