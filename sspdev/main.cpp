/*** 
 * @Author: LZH
 * @Date: 2022-05-30 19:25:46
 * @LastEditTime: 2022-06-12 16:32:35
 * @Description: 
 * @FilePath: /MyFiles/99_my-social-platform/sspdev/main.cpp
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "time.h"

#include "UserManager.h"
#include "RelationManager.h"
#include "MessageManager.h"
#include "PhotoManager.h"
#include "Socket.h"
#include "common/proto.h"
#include "common/ret_value.h"
#include "proto/message_define.pb.h"
#include "DbManager.h"
#include "BusManager.h"
#include "Session.h"

ssp::CommonReq common_req;
ssp::CommonRsp common_rsp;
ssp::ServerToUserReq inner_req;
ssp::UserToServerRsp inner_rsp;
SspSocket sock;
UserManager user_svr;
RelationManager rela_svr;
MessageManager mess_svr;
PhotoManager photo_svr;
DbManager db_svr;
BusManager bus;
int server_id=21;
int user_svr_count=2;
int user_svr_id[2]={11,12};
Session session;
int main(){
	bus.Init();
	bus.ChannelsClear();
	db_svr.Init();
	user_svr.Start(&db_svr);
	rela_svr.Start();
	mess_svr.Start();
	photo_svr.Start();
	sock.Init();
	sock.SocketInit();
	int server_on=1;
	while(server_on){

		int check_bus=bus.CheckRecv(server_id);
		int check_sock=-1;
		int cmd_id=-1;

		if(check_bus==-1){
			sock.SocketAccept();
			check_sock=sock.SocketCheckRecv();
			if(check_sock>0){
				printf("check_sock ok\n");
				common_req.ParseFromArray(sock.recv_buffer,10240);
				memset(sock.recv_buffer,0,common_req.ByteSize());
				cmd_id=common_req.header().cmd_type();
			}
		}else{
			printf("check_bus ok\n");
			inner_rsp.ParseFromArray(bus.Recv(check_bus,server_id),bus.RecvSize(check_bus,server_id));
			bus.Clear(check_bus,server_id);
			cmd_id=inner_rsp.header().cmd_type();
		}
		user_svr.Proc();
		rela_svr.Proc();
		mess_svr.Proc();
		photo_svr.Proc();
		int time_now=time(NULL);
		int ret=0;

		if(cmd_id<=0){
			usleep(5000);
			continue;
		}
		common_rsp.Clear();//todo 比较放在这里和放在分支下的效率
		common_rsp.mutable_header()->set_ver(1);
		common_rsp.mutable_header()->set_cmd_type(cmd_id+1);
			
		printf("----[debug]cmd_id:%d\n",cmd_id);
		switch(cmd_id){
			case REG_REQ:
			{
				common_rsp.mutable_reg_rsp()->Clear();
				//Send request to UserX
				strcpy(session.user_name,common_req.reg_req().user_name().c_str());
				inner_req.mutable_header()->set_ver(1);
				inner_req.mutable_header()->set_cmd_type(GET_USER_ID_REQ);
				inner_req.mutable_get_user_id()->set_user_name(common_req.reg_req().user_name());
				printf("----bytesize:%d\n",inner_req.ByteSize());
				inner_req.SerializeToArray(bus.send_buffer,inner_req.ByteSize());
				for(int i=0;i<user_svr_count;i++){
					printf("----%d %d ",server_id,user_svr_id[i]);
					for(int k=0;k<inner_req.ByteSize();k++){
						printf(" %02x",(unsigned char)bus.send_buffer[k]);
						if(k%16==8)printf("  ");
						if(k%16==0)printf("\n");
					}
					printf("\n");
					int ret_value=bus.Send( server_id,
											user_svr_id[i],
											bus.send_buffer,
											inner_req.ByteSize());
					printf("----bus_send_ret:%d\n",ret_value);
				}
				printf("----send all_user_server done\n");
				session.collect_count=0;
				session.cur_user_id=0x3fffffff;
			}
			break;
			case GET_USER_ID_RSP:
			{
				if(session.collect_count==user_svr_count){
					//drop
				}
				session.collect_count++;
				if(inner_rsp.get_user_id().ret()==USER_EXIST){
					common_rsp.mutable_reg_rsp()->set_ret(USER_EXIST);
					common_rsp.mutable_reg_rsp()->set_user_id(inner_rsp.get_user_id().user_id());
					common_rsp.SerializeToArray(sock.send_buffer,10240);
					printf("----response create failed user_exist\n");
					sock.SocketSend(common_rsp.ByteSize());
					continue;
				}else{
					printf("----record cur_user_id:%d\n",inner_rsp.get_user_id().user_id());
					
					if(session.cur_user_id>inner_rsp.get_user_id().user_id()){
						session.cur_user_id=inner_rsp.get_user_id().user_id();
					}
				}
				if(session.collect_count==user_svr_count){
					printf("----collect_all_user_server_response\n");
					//user_svr.CreateUser
					int user_idx=session.cur_user_id%2;
					int cur_user_svr_id;
					if(user_idx==1){
						cur_user_svr_id=user_svr_id[0];
					}else{
						cur_user_svr_id=user_svr_id[1];
					}
					//send user_svr UserCreate
					inner_req.mutable_header()->set_cmd_type(CREATE_USER_REQ);
					inner_req.mutable_create_user()->set_user_name(session.user_name);
					inner_req.mutable_create_user()->set_password(session.password);
					inner_req.mutable_create_user()->set_from(session.from);
					inner_req.SerializeToArray(bus.send_buffer,inner_req.ByteSize());
					printf("----send inner_req create_user\n");
					bus.Send(server_id,cur_user_svr_id,bus.send_buffer,inner_req.ByteSize());
				}
			}
			break;
			case CREATE_USER_RSP:
			{
				common_rsp.Clear();
				common_rsp.mutable_header()->set_ver(1);
				common_rsp.mutable_header()->set_cmd_type(REG_RSP);
				common_rsp.mutable_reg_rsp()->set_ret(inner_rsp.create_user().ret());
				common_rsp.mutable_reg_rsp()->set_user_id(inner_rsp.create_user().user_id());
				common_rsp.SerializeToArray(sock.send_buffer,10240);
				sock.SocketSend(common_rsp.ByteSize());
			}
			break;
			case LOGIN_REQ:
			{
				common_rsp.mutable_login_rsp()->Clear();
				ret=user_svr.LoginCheck(common_req.login_req().user_name().c_str(),
										common_req.login_req().password().c_str());
				
				int user_id=user_svr.GetUserIdByUserName(common_req.login_req().user_name().c_str());
				int ret1=rela_svr.UserRelationInit(user_id);
				int ret2=photo_svr.CreatePhoto(user_id);
				printf("logincheck=%d rela_create=%d photo_create=%d\n",ret,ret1,ret2);
				common_rsp.mutable_login_rsp()->set_ret(ret);
				if(ret==SUCCESS){
					common_rsp.mutable_login_rsp()->set_user_id(user_id);
				}
				common_rsp.SerializeToArray(sock.send_buffer,10240);
				sock.SocketSend(common_rsp.ByteSize());
			}
			break;
			case ADD_FRIEND_REQ:
			{
				common_rsp.mutable_add_friend_rsp()->Clear();
				int user_id  = common_req.add_friend_req().user_id();
				int other_id = common_req.add_friend_req().other_id();
				ret=user_svr.CheckExist(user_id);
				if(ret==USER_NOT_EXIST){
					// USER_NOT_EXIST
				}else{
					ret=user_svr.CheckExist(other_id);
					if(ret==USER_EXIST){
						ret=rela_svr.AddFriend(user_id,other_id);
					}else{
						//FRIEND_ID NOT EXIST
					}
				}
				common_rsp.mutable_add_friend_rsp()->set_ret(ret);
				common_rsp.SerializeToArray(sock.send_buffer,10240);
				sock.SocketSend(common_rsp.ByteSize());
			}
			break;
			case PUBLISH_MESSAGE_REQ:
			{
				common_rsp.mutable_publish_message_rsp()->Clear();
				int user_id=common_req.publish_message_req().user_id();
				int mess_id_ret=0;
				ret=user_svr.CheckExist(user_id);
				if(ret==USER_EXIST){
					MessageInfo message;
					message.set_user_id(user_id);
					message.set_content(common_req.publish_message_req().content().c_str());
					message.set_publish_time(time_now);
					mess_id_ret=mess_svr.PublishMessage(message);
					printf("message_publish=%d\n",mess_id_ret);
				}
				if(mess_id_ret!=MESSAGE_TOO_MUCH){
					RelationInfo* rela_info=rela_svr.GetRelation(user_id);
					if(rela_info){
						printf("friend_count=%d\n",rela_info->friend_count());
						for(int i=0;i<rela_info->friend_count();i++){
							int friend_id=rela_info->GetFriendByIndex(i);
							photo_svr.UpdatePhoto(friend_id,user_id,time_now,0);
							mess_svr.PushUserMessageId(friend_id,mess_id_ret);
						}
					}
				}
				common_rsp.mutable_publish_message_rsp()->set_ret(ret);
				common_rsp.SerializeToArray(sock.send_buffer,10240);
				sock.SocketSend(common_rsp.ByteSize());
			}	
			break;
			case GET_PHOTO_REQ:
			{
				common_rsp.mutable_get_photo_rsp()->Clear();
				int user_id=common_req.get_photo_req().user_id();
				ret=user_svr.CheckExist(user_id);
				if(ret==USER_EXIST){
					PhotoInfo* photo = photo_svr.GetPhoto(user_id);
					if(photo!=NULL){
						printf("last_publisher_id: %d\n",photo->last_publisher_id());
						common_rsp.mutable_get_photo_rsp()->set_last_publisher_id(photo->last_publisher_id());
					}else{
						printf("photo info is null\n");
					}
				}
				common_rsp.mutable_get_photo_rsp()->set_ret(ret);
				common_rsp.SerializeToArray(sock.send_buffer,10240);
				sock.SocketSend(common_rsp.ByteSize());
			}
			break;
			case GET_MESSAGE_LIST_REQ:
			{
				common_rsp.mutable_get_message_list_rsp()->Clear();
				int user_id=common_req.get_message_list_req().user_id();
				ret=user_svr.CheckExist(user_id);
				if(ret==USER_EXIST){
					vector<int> ids=mess_svr.GetMessageIds(user_id);
					for(int i=0;i<ids.size()&&i<10;i++){
						MessageInfo* message=mess_svr.GetMessage(ids[i]);
						ssp::MessageItem* item=common_rsp.mutable_get_message_list_rsp()->add_message_list();
						item->set_publisher_id(message->user_id());
						item->set_publish_time(message->publish_time());
						item->set_content(message->content());
					}
				}
				common_rsp.mutable_get_message_list_rsp()->set_ret(ret);
				common_rsp.SerializeToArray(sock.send_buffer,10240);
				sock.SocketSend(common_rsp.ByteSize());
			}
			break;
			case DEL_FRIEND_REQ:
			{
				common_rsp.mutable_del_friend_rsp()->Clear();
				int user_id  = common_req.del_friend_req().user_id();
				int other_id = common_req.del_friend_req().other_id();
				ret=user_svr.CheckExist(user_id);
				if(ret==USER_NOT_EXIST){
					// USER_NOT_EXIST
				}else{
					ret =user_svr.CheckExist(other_id);
					if(ret==USER_EXIST){
						ret=rela_svr.DelFriend(user_id,other_id);
					}else{
						//FRIEND_ID NOT EXIST
					}
				}
				common_rsp.mutable_del_friend_rsp()->set_ret(ret);
				common_rsp.SerializeToArray(sock.send_buffer,10240);
				sock.SocketSend(common_rsp.ByteSize());
			}
			break;
			case LOGOUT_REQ:
			{
				common_rsp.Clear();
				common_rsp.mutable_header()->set_cmd_type(LOGOUT_RSP);
				common_rsp.mutable_logout_rsp()->set_ret(SUCCESS);
				common_rsp.SerializeToArray(sock.send_buffer,10240);
				sock.SocketSend(common_rsp.ByteSize());
				sock.ClientClose();
			}
			break;
			case SERVER_SHUTDOWN:
			{
				server_on = 0;
			}
			break;
			default:
			break;
		}
	}
	user_svr.Shutdown();
	rela_svr.Shutdown();
	mess_svr.Shutdown();
	photo_svr.Shutdown();
	return 0;
}
