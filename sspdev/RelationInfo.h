/*** 
 * @Author: LZH
 * @Date: 2022-05-26 19:25:46
 * @LastEditTime: 2022-05-27 14:53:07
 * @Description: 
 * @FilePath: /MyFiles/99_my-social-platform/sspdev/RelationInfo.h
 */
#include "string.h"
#include "common/macro.h"

class RelationInfo{
private:
	GETSETVAR(int,user_id)
	GETSETVAR(int,friend_count)
	int friend_list_[10240];
	GETSETVAR(int,black_count)
	int black_list_[10240];
public:
	int CheckFriend(int other_id);
	int CheckBlack(int other_id);
	int AddFriend(int other_id);
	int AddBlack(int other_id);
	int DeleteFriend(int other_id);
	int DeleteBlack(int other_id);
	int GetFriendByIndex(int index);
	int GetBlackByIndex(int index);

};






