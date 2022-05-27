/*** 
 * @Author: LZH
 * @Date: 2022-05-26 19:25:46
 * @LastEditTime: 2022-05-27 14:42:09
 * @Description: 
 * @FilePath: /MyFiles/99_my-social-platform/sspdev/UserInfo.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UserManager.h"
#include "common/ret_value.h"


int UserManager::Start(DbManager* db_svr){
	db_svr_ = db_svr;
	//Get Current User ID from DB
	set_cur_user_id(db_svr_->GetCurUserId());
	printf("cur_user_id: %d\n",cur_user_id());
	set_user_count(0);
	//read users from DB;
	int ret=db_svr_->GetUsersBegin();
	int user_i=0;
	if(ret==SUCCESS){
		ret=db_svr_->GetUsersOneByOne(&users_[user_i]);
		if(ret==SUCCESS){
			//printf("%d %s %s\n",users_[user_i].user_id(),users_[user_i].user_name(),users_[user_i].password());
			user_i++;
		}
		while(ret!=DB_NO_MORE_DATA){
			ret=db_svr_->GetUsersOneByOne(&users_[user_i]);
			if(ret==SUCCESS){
				//printf("%d %s %s\n",users_[user_i].user_id(),users_[user_i].user_name(),users_[user_i].password());
				user_i++;
			};
		}
		ret=db_svr_->GetUsersEnd();
		set_user_count(user_i);
		printf("UserCount:%d\n",user_count_);
	}else{
		printf("UserManager Start Failed(Db Get User Failed: %d)\n",ret);
		return ret;
	}
	printf("UserManager Start\n");
	return SUCCESS;
}

int UserManager::Proc(){
	return SUCCESS;
}

int UserManager::Restart(){
	return SUCCESS;
}

int UserManager::Shutdown(){
	return SUCCESS;
}
int UserManager::CheckExistB(int user_id){
	int l=0,r=user_count_-1;
	int mid=(l+r)/2;
	while(l<=r){
		mid=(l+r)/2;
		if(users_[mid].user_id()==user_id){
			break;
		}
		if(users_[mid].user_id()<user_id){
			l=mid+1;
		}else{
			r=mid-1;
		}
	}
	if(users_[mid].user_id()==user_id){
		return USER_EXIST;
	}
	return USER_NOT_EXIST;
}
int UserManager::CheckExist(int user_id){
	for(int i=0;i<user_count_;i++){
		if(users_[i].user_id()==user_id){
			return USER_EXIST;
		}
	}
	return USER_NOT_EXIST;
}

UserInfo* UserManager::GetUser(int user_id){
	for(int i=0;i<user_count_;i++){
		if(users_[i].user_id()==user_id){
			return &users_[i];
		}

	}
	return NULL;
}


int UserManager::CreateUser(const char* user_name,const char* password, int from, int reg_time){
	int user_index_saved=-1;
	for(int i=0;i<user_count_;i++){
		if(strcmp(users_[i].user_name(),user_name)==0){
			return USER_EXIST;
		}
	}
	
	if(user_count_<10239){
		users_[user_count_].set_user_id(cur_user_id());
		users_[user_count_].set_user_name(user_name);
		users_[user_count_].set_password(password);
		users_[user_count_].set_from(from);
		users_[user_count_].set_reg_time(reg_time);
		users_[user_count_].set_db_flag(FLAG_INSERT);
		user_index_saved = user_count_;	
		user_count_++;
	}else{
		return USER_TOO_MUCH;
	}
	int ret_user_id=cur_user_id();
	set_cur_user_id(cur_user_id()+1);
	printf("changed!cur_user_id: %d\n",cur_user_id());
	//SaveUser()
	db_svr_->InsertUser(&users_[user_index_saved]);
	return ret_user_id;	
}

int UserManager::DeleteUser(int user_id){
	for(int i=0;i<user_count_;i++){
		if(users_[i].user_id()==user_id){
			users_[i].set_db_flag(FLAG_DELETE);
			return SUCCESS;
		}
	}
	return USER_NOT_EXIST;
}

int UserManager::LoginCheck(const char* user_name,const char* password){
	for(int i=0;i<user_count_;i++){
		if(strcmp(users_[i].user_name(),user_name)==0){
			if(strcmp(users_[i].password(),password)==0){
				return SUCCESS;
			}
			else{
				return WRONG_PASSWORD;
			}
		}
	}
	return USER_NOT_EXIST;
}

int UserManager::UserLogout(int user_id,int time_now){
	for(int i=0;i<user_count_;i++){
		if(users_[i].user_id()==user_id){
			users_[i].set_logout_time(time_now);
			users_[i].set_db_flag(FLAG_UPDATE);
		}
		return SUCCESS;
	}
	return USER_NOT_EXIST;
}

int UserManager::UpdateLoginTime(int user_id,int time_now){
	UserInfo* user=GetUser(user_id);
	if(user==NULL){
		return USER_NOT_EXIST;
	}
	user->set_login_time(time_now);
	user->set_db_flag(FLAG_UPDATE);
	return SUCCESS;
}

int UserManager::GetUserIdByUserName(const char* user_name){
	for(int i=0;i<user_count_;i++){
		if(strcmp(users_[i].user_name(),user_name)==0){
			return users_[i].user_id();
		}
	}
	return USER_NOT_EXIST;
}
