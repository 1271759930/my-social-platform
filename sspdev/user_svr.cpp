/*** 
 * @Author: LZH
 * @Date: 2022-05-26 19:25:46
 * @LastEditTime: 2022-06-12 22:01:21
 * @Description: 
 * @FilePath: /MyFiles/99_my-social-platform/sspdev/user_svr.cpp
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BusManager.h"
#include "common/ret_value.h"
#include "common/proto.h"
#include "proto/message_define.pb.h"
#include "UserManager.h"
#include "DbManager.h"


BusManager bus;
UserManager user_svr;
DbManager db_svr;
int user_svr_id=0;// 1 or 2
ssp::ServerToUserReq inner_req;
ssp::UserToServerRsp inner_rsp;

void MAKE_RSP_HEAD(int cmd_id){
	inner_rsp.Clear();
	inner_rsp.mutable_header()->set_ver(1);
	inner_rsp.mutable_header()->set_cmd_type(cmd_id+1);
}

void BUS_SEND_RSP(int from,int to){
	inner_rsp.SerializeToArray(bus.send_buffer,inner_rsp.ByteSize());
	printf("----[debug]bus send message:\n");
	string debug_message=inner_rsp.Utf8DebugString();
	printf("%s\n",debug_message.c_str());
	int ret=bus.Send(from,to,bus.send_buffer,inner_rsp.ByteSize());
	printf("send_ret:%d\n",ret);
}
int main(int argc, char* argv[]){
	if(argc<2){
		printf("Usage: user_svr <svr_id>");
		return 0;
	}

	sscanf(argv[1],"%d",&user_svr_id);
	bus.Init();
	db_svr.Init();
	user_svr.Start(&db_svr);
	int server_on=1;
	while(server_on){
		int time_now=time(NULL);
		int check_bus=bus.CheckRecv(user_svr_id);
		if(check_bus!=-1){
			// ---- bus check recv; after recv clear bus
			inner_req.ParseFromArray(bus.Recv(check_bus,user_svr_id),bus.RecvSize(check_bus,user_svr_id));
			printf("----[debug]bus recv message:\n");
			string debug_message=inner_req.Utf8DebugString();
			printf("%s\n",debug_message.c_str());
			bus.Clear(check_bus,user_svr_id);

			int cmd_id=inner_req.header().cmd_type();
			switch(cmd_id){
				case GET_USER_ID_REQ:
				{
					MAKE_RSP_HEAD(cmd_id);
					int user_id=user_svr.GetUserIdByUserName(inner_req.get_user_id().user_name().c_str());
					if(user_id==USER_EXIST){
						inner_rsp.mutable_get_user_id()->set_ret(USER_EXIST);
					}else{
						inner_rsp.mutable_get_user_id()->set_ret(USER_NOT_EXIST);
						inner_rsp.mutable_get_user_id()->set_user_id(user_svr.cur_user_id());
					}

					BUS_SEND_RSP(user_svr_id,check_bus);
				}
				break;
				case CREATE_USER_REQ:
				{
					MAKE_RSP_HEAD(cmd_id);
					// ---- add code here ----
					int user_id=user_svr.CreateUser(inner_req.create_user().user_name().c_str(), 
								 				inner_req.create_user().password().c_str(), 
								 				inner_req.create_user().from(), time_now);

					if(user_id==USER_EXIST){
						inner_rsp.mutable_create_user()->set_ret(user_id);
						inner_rsp.mutable_create_user()->set_user_id(user_id);
					}else{
						inner_rsp.mutable_create_user()->set_ret(SUCCESS);
						inner_rsp.mutable_create_user()->set_user_id(user_id);
					}
					BUS_SEND_RSP(user_svr_id,check_bus);
				}
				break;
				case LOGIN_CHECK_REQ:
					MAKE_RSP_HEAD(cmd_id);
					// ---- add code here ----
					BUS_SEND_RSP(user_svr_id,check_bus);
				break;
				case CHECH_EXIST_REQ:
					MAKE_RSP_HEAD(cmd_id);
					// ---- add code here ----
					BUS_SEND_RSP(user_svr_id,check_bus);
				break;
				default:
					break;
			}
		}
	}
	bus.Detach();
	return 0;
}
