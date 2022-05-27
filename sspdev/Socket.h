/*** 
 * @Author: LZH
 * @Date: 2022-05-26 19:25:46
 * @LastEditTime: 2022-05-27 14:51:56
 * @Description: 
 * @FilePath: /MyFiles/99_my-social-platform/sspdev/Socket.h
 */

#ifndef _SOCKET_H
#define _SOCKET_H
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "Config.h"
class SspSocket{
private:
	int use_socket;
	int rs_flag;
public:
	int server_sock;
	struct sockaddr_in server_addr;
	int client_sock;
	struct sockaddr_in client_addr;
	int accept_flag;
	char recv_buffer[10240];
	char send_buffer[10240];
	Config config;
	int Init();
	int SocketInit();
	int SocketAccept();
	int SocketCheckRecv();
	int SocketSend(int send_size);
	int ClientClose();
	int ServerClose();
	int ClientSocketInit();
	int show_send_buffer(int size);
	int show_recv_buffer(int size);
};
#endif
