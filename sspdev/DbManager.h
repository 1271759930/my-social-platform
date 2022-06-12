/*** 
 * @Author: LZH
 * @Date: 2022-05-30 19:25:46
 * @LastEditTime: 2022-06-12 16:33:38
 * @Description: 
 * @FilePath: /MyFiles/99_my-social-platform/sspdev/DbManager.h
 */


#ifndef _DBMANAGER_H
#define _DBMANAGER_H
#include "mysql/mysql.h"
#include <string>
#include <iostream>
#include "UserInfo.h"

using namespace std;

class DbManager {
private:
	GETSETVAR(int,transection)
private:
	int mysql_defined;
	#ifndef NO_MYSQL
	MYSQL *conn;
	MYSQL_RES *result;
	MYSQL_ROW row;
	#endif
public:
	int Init();
	int execSql(string sql);
	int GetUsersBegin();
	int GetUsersOneByOne(UserInfo* user);
	int GetUsersEnd();
	int GetCurUserId();
	int InsertUser(UserInfo* user);
};

#endif
