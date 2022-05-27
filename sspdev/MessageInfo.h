/*** 
 * @Author: LZH
 * @Date: 2022-05-26 19:25:46
 * @LastEditTime: 2022-05-27 14:53:44
 * @Description: 
 * @FilePath: /MyFiles/99_my-social-platform/sspdev/MessageInfo.h
 */

#include "string.h"
#include "common/macro.h"

class MessageInfo {
	GETSETVAR(int,user_id)
	GETSETVAR(int,message_id)
	GETSETVAR(int,publish_time)
	GETSETSTR(1024,content)
};
