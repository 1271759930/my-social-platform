/*** 
 * @Author: LZH
 * @Date: 2022-05-26 19:25:46
 * @LastEditTime: 2022-05-27 14:52:23
 * @Description: 
 * @FilePath: /MyFiles/99_my-social-platform/sspdev/Socket.cpp
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Socket.h"
#define UES_SOCKET_CHECK if(use_socket==0){return 0;}

#define NONE 0
#define RECV 1
#define SEND 2

int SspSocket::Init(){
	rs_flag=NONE;
	accept_flag=0;
	printf("SocketInit\n");
	config.ReadConfig();
	use_socket=config.use_socket();//0 - do not use socket; 1 - nomarl
	return 0;
}
int SspSocket::SocketInit(){
	UES_SOCKET_CHECK

	int ret;
	server_sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	printf("socket:%d",server_sock);
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port= htons(config.socket_port());
	server_addr.sin_addr.s_addr = inet_addr(config.socket_addr());
	ret=bind(server_sock , (struct sockaddr*)&server_addr,sizeof(server_addr));
	printf("-bind:%d",ret);
	ret=listen(server_sock, 10000);
	printf("-listen:%d\n",ret);
	return 0;
}
int SspSocket::ClientSocketInit(){
	UES_SOCKET_CHECK

	client_sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	struct sockaddr_in server_addr;
	memset(&server_addr, 0 ,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(config.socket_addr());
	server_addr.sin_port = htons(config.socket_port());
	int conn_ret=connect(client_sock, (struct sockaddr*)&server_addr,sizeof(server_addr));
	if(conn_ret<0){
		return conn_ret;
	}
	accept_flag = 1;
	return 0;
}
int SspSocket::SocketAccept(){
	UES_SOCKET_CHECK
	//printf("accept in\n");
	if(accept_flag==1){
	//	printf("accept 1\n");
		return 0;
	}
	//printf("accept go on\n");
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size = sizeof(clnt_addr);
	client_sock = accept(server_sock , (struct sockaddr*)&clnt_addr, &clnt_addr_size);
	accept_flag=1;
	return 0;
}

int SspSocket::SocketCheckRecv(){
	UES_SOCKET_CHECK

	if(accept_flag==0){
		return -1;
	}

	if(rs_flag==RECV){
		return 0;
	}
	printf("do recv\n");
	int ret=recv(client_sock,recv_buffer,10240,0);
	printf("recv ok\n");
	if(ret>0){
		rs_flag=RECV;
		return ret;
	}else{
		return -1;
	}
	return 0;
}

int SspSocket::SocketSend(int send_size){
	UES_SOCKET_CHECK

	int ret=send(client_sock,send_buffer,send_size,0);
	memset(send_buffer,0,send_size);
	rs_flag=SEND;
	if(ret<0){
		ClientClose();
		return -1;
	}
	return 0;
}
int SspSocket::ClientClose(){
	UES_SOCKET_CHECK

	close(client_sock);
	accept_flag = 0;
	return 0;
}

int SspSocket::ServerClose(){
	UES_SOCKET_CHECK
	
	close(server_sock);
	return 0;
}

int SspSocket::show_send_buffer(int size){
	for(int i=0;i<size;i++){
		int l4=send_buffer[i]&0x000f;
		int h4=(send_buffer[i]>>4)&0x000f;
		int l0=l4&0x0001;
		l4=l4>>1;
		int l1=l4&0x0001;
		l4=l4>>1;
		int l2=l4&0x0001;
		l4=l4>>1;
		int l3=l4&0x0001;
		l3=l4&0x0001;

		int h0=h4&0x0001;
		h4=h4>>1;
		int h1=h4&0x0001;
		h4=h4>>1;
		int h2=h4&0x0001;
		h4=h4>>1;
		int h3=h4&0x0001;
		printf("%02x %d%d%d%d %d%d%d%d %3d",send_buffer[i],h3,h2,h1,h0,l3,l2,l1,l0,send_buffer[i]);
		if('0'<=send_buffer[i]&&send_buffer[i]<='9')printf("'%c'",send_buffer[i]);
		if('A'<=send_buffer[i]&&send_buffer[i]<='Z')printf("'%c'",send_buffer[i]);
		if('a'<=send_buffer[i]&&send_buffer[i]<='z')printf("'%c'",send_buffer[i]);
		printf("\n");
	}
	return 0;
}

int SspSocket::show_recv_buffer(int size){
	for (int i=0;i<size;i++){
		int l4=recv_buffer[i]&0x000f;
		int h4=(recv_buffer[i]>>4)&0x000f;
		int l0=l4&0x0001;
		l4=l4>>1;
		int l1=l4&0x0001;
		l4=l4>>1;
		int l2=l4&0x0001;
		l4=l4>>1;
		int l3=l4&0x0001;
		int h0=h4&0x0001;
		h4=h4>>1;
		int h1=h4&0x0001;
		h4=h4>>1;
		int h2=h4&0x0001;
		h4=h4>>1;
		int h3=h4&0x0001;
		printf("%02x %d%d%d%d %d%d%d%d %3d ",recv_buffer[i],h3,h2,h1,h0,l3,l2,l1,l0,recv_buffer[i]);
		if('0'<=recv_buffer[i]&&recv_buffer[i]<='9')printf("'%c'",recv_buffer[i]);
		if('A'<=recv_buffer[i]&&recv_buffer[i]<='Z')printf("'%c'",recv_buffer[i]);
		if('a'<=recv_buffer[i]&&recv_buffer[i]<='z')printf("'%c'",recv_buffer[i]);
		printf("\n");
	}
	return 0;
}
