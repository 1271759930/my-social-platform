/*** 
 * @Author: LZH
 * @Date: 2022-05-26 19:25:46
 * @LastEditTime: 2022-05-27 14:52:53
 * @Description: 
 * @FilePath: /MyFiles/99_my-social-platform/sspdev/RelationManager.cpp
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common/ret_value.h"
#include "RelationManager.h"
int RelationManager::Start(){
	//printf("[  INFO  ]Relation Manager Start\n");
	return 0;
}
int RelationManager::Proc(){
	//printf("Relation Manager Proc\n");
	return 0;
}

int RelationManager::Shutdown(){
	//printf("[  INFO  ]Relation Manager Shutdown\n");
	return 0;
}

int RelationManager::Restart(){
//	printf("[  INFO  ]Relation Manager Restart\n");
	return 0;
}

int RelationManager::UserRelationInit(int user_id){
	for(int i=0;i<relation_count_;i++){
		if(relations_[i].user_id()==user_id){
			return RELATION_EXIST;
		}
	}
	if(relation_count_<10239){
		relations_[relation_count_].set_user_id(user_id);
		relation_count_++;
		return SUCCESS;
	}else{
		return RELATION_TO_MUCH;
	}
}

RelationInfo* RelationManager::GetRelation(int user_id){
	for(int i=0;i<relation_count_;i++){
		if(relations_[i].user_id()==user_id){
			return &relations_[i];
		}
	}
	return NULL;
}

int RelationManager::AddFriend(int user_id,int other_id){
	RelationInfo* relation=GetRelation(user_id);
	if(relation==NULL){
		return RELATION_NOT_EXIST;
	}
	int ret=relation->AddFriend(other_id);
	return ret;
}

int RelationManager::DelFriend(int user_id,int other_id){
	RelationInfo* relation=GetRelation(user_id);
	if(relation==NULL){
		return RELATION_NOT_EXIST;
	}
	int ret=relation->DeleteFriend(other_id);
	return ret;
}
