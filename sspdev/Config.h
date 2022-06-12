/*** 
 * @Author: LZH
 * @Date: 2022-05-26 19:25:46
 * @LastEditTime: 2022-06-12 21:39:11
 * @Description: 
 * @FilePath: /MyFiles/99_my-social-platform/sspdev/Config.h
 */

#ifndef _CONFIG_H
#define _CONFIG_H
#include "common/macro.h"
class Config{
	GETSETVAR(int,use_socket)
	GETSETVAR(int,socket_port)
	GETSETSTR(256,socket_addr)
public:
	int ReadConfig();
};
#endif
