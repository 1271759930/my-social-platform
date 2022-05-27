/*** 
 * @Author: LZH
 * @Date: 2022-05-26 19:25:46
 * @LastEditTime: 2022-05-27 14:42:09
 * @Description: 
 * @FilePath: /MyFiles/99_my-social-platform/sspdev/UserInfo.h
 */

#ifndef _USER_INFO_H_
#define _USER_INFO_H_

#include "string.h"
#include "common/macro.h"

#include "proto/user_info_base.pb.h"
class UserInfo{
	public:
		int FromPb(ssp::UserInfoBase &pb_user);
		int ToPb(ssp::UserInfoBase &pb_user);
	private:
		GETSETVAR(int,user_id)
		GETSETSTR(256,user_name)
		GETSETSTR(256,nick_name)
		GETSETVAR(int,reg_time)
		GETSETVAR(int,from)
		GETSETVAR(int,login_time)
		GETSETVAR(int,last_login_time)
		GETSETVAR(int,fresh_time)
		GETSETSTR(256,password)
		GETSETVAR(int,logout_time)
		GETSETVAR(int,db_flag)
};

#endif
