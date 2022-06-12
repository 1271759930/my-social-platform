/*
 * @Author: LZH
 * @Date: 2022-05-26 19:25:46
 * @LastEditTime: 2022-06-04 22:07:34
 * @Description: 
 * @FilePath: /MyFiles/99_my-social-platform/sspdev/2client.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "Socket.h"
#include "common/proto.h"
#include "proto/message_define.pb.h"

#define GOTO_BEGIN if(ret==0){phase=1;}
#define GOTO_NEXT if(ret==0){phase++;}
#define GOTO_LOGIN if(ret==0){phase=5;}
#define GET_OUT if(ret==0){phase=999;}
#define WAIT_RSP_GO_NEXT ret=RecvRsp();GOTO_NEXT
#define WAIT_RSP_GO_LOGIN ret=RecvRsp();GOTO_LOGIN
#define WAIT_RSP_GO_BEGIN ret=RecvRsp();GOTO_BEGIN
#define WAIT_RSP_GET_OUT ret=RecvRsp();GET_OUT
#define CHECK_SERVER_ON if(ret<0){\
							printf("Server Not Ready\n");\
							return 0;\
						}

#define SEND	common_req.SerializeToArray(sock.send_buffer,common_req.ByteSize());\
				int ret=sock.SocketSend(common_req.ByteSize());\
				return ret;

int debug_on=1;
ssp::CommonReq common_req;
ssp::CommonRsp common_rsp;

SspSocket sock;

int Register(const char *user_name,const char *password){
	common_req.Clear();
	common_req.mutable_header()->set_ver(1);
	common_req.mutable_header()->set_cmd_type(REG_REQ);
	common_req.mutable_reg_req()->set_user_name(user_name);
	common_req.mutable_reg_req()->set_password(password);
	common_req.mutable_reg_req()->set_from(1);
	SEND
}
int Login(const char *user_name,const char* password){
	common_req.Clear();
	common_req.mutable_header()->set_ver(1);
	common_req.mutable_header()->set_cmd_type(LOGIN_REQ);
	common_req.mutable_login_req()->set_user_name(user_name);
	common_req.mutable_login_req()->set_password(password);
	SEND
}
int Logout(int user_id){
	common_req.Clear();
	common_req.mutable_header()->set_ver(1);
	common_req.mutable_header()->set_cmd_type(LOGOUT_REQ);
	common_req.mutable_logout_req()->set_user_id(user_id);
	SEND
}
int AddFriend(int user_id,int other_id){
	common_req.Clear();
	common_req.mutable_header()->set_ver(1);
	common_req.mutable_header()->set_cmd_type(ADD_FRIEND_REQ);
	common_req.mutable_add_friend_req()->set_user_id(user_id);
	common_req.mutable_add_friend_req()->set_other_id(other_id);
	SEND
}
int DelFriend(int user_id,int other_id){
	common_req.Clear();
	common_req.mutable_header()->set_ver(1);
	common_req.mutable_header()->set_cmd_type(DEL_FRIEND_REQ);
	common_req.mutable_del_friend_req()->set_user_id(user_id);
	common_req.mutable_del_friend_req()->set_other_id(other_id);
	SEND
}
int PublishMessage(int user_id,const char * content){
	common_req.Clear();
	common_req.mutable_header()->set_ver(1);
	common_req.mutable_header()->set_cmd_type(PUBLISH_MESSAGE_REQ);
	common_req.mutable_publish_message_req()->set_user_id(user_id);
	common_req.mutable_publish_message_req()->set_content(content);
	SEND
}
int GetPhoto(int user_id){
	common_req.Clear();
	common_req.mutable_header()->set_ver(1);
	common_req.mutable_header()->set_cmd_type(GET_PHOTO_REQ);
	common_req.mutable_get_photo_req()->set_user_id(user_id);
	SEND
}
int GetMessageList(int user_id){
	common_req.Clear();
	common_req.mutable_header()->set_ver(1);
	common_req.mutable_header()->set_cmd_type(GET_MESSAGE_LIST_REQ);
	common_req.mutable_get_message_list_req()->set_user_id(user_id);
	SEND
}
int ShutDown(){
	common_req.Clear();
	common_req.mutable_header()->set_ver(1);
	common_req.mutable_header()->set_cmd_type(SERVER_SHUTDOWN);
	common_req.SerializeToArray(sock.send_buffer,common_req.ByteSize());
	int ret=sock.SocketSend(common_req.ByteSize());
	return ret;
}
int saved_user_id1=0;
int saved_user_id2=0;

int RecvRsp(){
	int ret=sock.SocketCheckRecv();
	if(ret>0){
		common_rsp.ParseFromArray(sock.recv_buffer,10240);
		int cmd_type=common_rsp.header().cmd_type();
		if(debug_on==0)return 0;
		switch(cmd_type){
			case REG_RSP:
				printf("Receive Reg Rsp\n");
				printf("ret    : %d\n",common_rsp.reg_rsp().ret());
				printf("user_id: %d\n",common_rsp.reg_rsp().user_id());
			break;
			case LOGIN_RSP:
				printf("Receive Login Rsp\n");
				printf("ret    : %d\n",common_rsp.login_rsp().ret());
				printf("user_id: %d\n",common_rsp.login_rsp().user_id());
				if(saved_user_id1==0){
					saved_user_id1=common_rsp.login_rsp().user_id();
				}else{
					if(saved_user_id2==0){
						saved_user_id2=common_rsp.login_rsp().user_id();
					}
				}
			break;
			case ADD_FRIEND_RSP:
				printf("Receive AddFriend Rsp\n");
				printf("ret    : %d\n",common_rsp.add_friend_rsp().ret());
			break;
			case DEL_FRIEND_RSP:
				printf("Receive DelFriend Rsp\n");
				printf("ret    : %d\n",common_rsp.del_friend_rsp().ret());
			break;
			case PUBLISH_MESSAGE_RSP:
				printf("Receive PublishMessage Rsp\n");
				printf("ret    : %d\n",common_rsp.publish_message_rsp().ret());
			break;
			case GET_PHOTO_RSP:
				printf("Receive GetPhoto Rsp\n");
				printf("ret    : %d\n",common_rsp.get_photo_rsp().ret());
				printf("update : %d\n",common_rsp.get_photo_rsp().last_publisher_id());
			break;
			case GET_MESSAGE_LIST_RSP:
			{
				printf("Receive GetMessageList Rsp\n");
				printf("ret    : %d\n",common_rsp.get_message_list_rsp().ret());
				int message_count=common_rsp.get_message_list_rsp().message_list_size();
				printf("count  : %d\n",common_rsp.get_message_list_rsp().message_list_size());
				for(int i=0;i<message_count;i++){
					printf("%6d : %s\n",common_rsp.get_message_list_rsp().message_list(i).publisher_id(),
		   								common_rsp.get_message_list_rsp().message_list(i).content().c_str());
				}
			}	
			break;
			default:
			break;
		}
	}else{
		return -1;
	}
	return 0;
}

int main(){
	int ret;
	ret=sock.Init();
	ret=sock.ClientSocketInit();
	CHECK_SERVER_ON
	int client_on=1;
	int phase=1;
	while(client_on){
		switch(phase){
			case 1:
				ret=Register("hank1234","12345678");
				GOTO_NEXT
				break;
			case 2:
				WAIT_RSP_GO_NEXT
				break;
			default:
				ret=Logout(saved_user_id1);
				//ret=ShutDown();
				//GOTO_NEXT
				client_on=0;
			break;
		}
	}
	sock.ClientClose();
	return 0;
}
