/*** 
 * @Author: LZH
 * @Date: 2022-05-26 19:25:46
 * @LastEditTime: 2022-05-27 14:52:42
 * @Description: 
 * @FilePath: /MyFiles/99_my-social-platform/sspdev/RelationManager.h
 */

#ifndef _RELATIONMANAGER_H
#define _RELATIONMANAGER_H
#include "RelationInfo.h"
class RelationManager{
	private:
		RelationInfo relations_[10240];
		GETSETVAR(int,relation_count)
	public:
		int Start();
		int Proc();
		int Restart();
		int Shutdown();
	public:
		int UserRelationInit(int user_id);
		RelationInfo* GetRelation(int user_id);
		int AddFriend(int user_id,int other_id);
		int DelFriend(int user_id,int other_id);
		//AddBlack
		//DelBlack

};
#endif
