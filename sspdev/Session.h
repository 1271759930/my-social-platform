/*** 
 * @Author: LZH
 * @Date: 2022-05-26 19:25:46
 * @LastEditTime: 2022-06-12 21:49:25
 * @Description: 
 * @FilePath: /MyFiles/99_my-social-platform/sspdev/Session.h
 */

#ifndef _SESSION_H
#define _SESSION_H
class Session{
	public:
	char user_name[256];
	char password[256];
	int from;
	int cur_user_id;
	int collect_count;
};
#endif
