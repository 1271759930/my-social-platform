/*** 
 * @Author: LZH
 * @Date: 2022-05-30 19:25:46
 * @LastEditTime: 2022-06-12 16:34:02
 * @Description: 
 * @FilePath: /MyFiles/99_my-social-platform/sspdev/DbManager.cpp
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DbManager.h"

#include "common/ret_value.h"

char mysql_username[256];
char mysql_password[256];

int DbManager::Init(){
	FILE* f=fopen("mysql.ini","r");
	fscanf(f,"%s",mysql_username);
	fscanf(f,"%s",mysql_password);
	fclose(f);

	conn=mysql_init(NULL);
	if(conn==NULL){
		return DB_CONN_INIT_FAIL;
	}
	conn = mysql_real_connect(conn, "127.0.0.1",mysql_username,mysql_password,"ssp",0,NULL,0);
	if(conn==NULL){
		return DB_CONN_CONNECT_FAIL;
	}
	return SUCCESS;
}

int DbManager::execSql(string sql){
	if(mysql_query(conn,sql.c_str())){
		return DB_QUERY_FAIL;
	}else{
		result = mysql_use_result(conn);
		if(result){
			int field_count = mysql_num_fields(result);
			int row_count = mysql_num_rows(result);
			printf("rows: %d, fields: %d\n",row_count,field_count);
			while(result){
				row = mysql_fetch_row(result);
				if(row == NULL){
					printf("No More Row\n");
					break;
				}
				for(int j=0;j<field_count;j++){
					printf("%s\t",row[j]);
				}
				printf("\n");
			}
			row_count = mysql_num_rows(result);
			printf("rows: %d, fields: %d\n",row_count,field_count);

		}
		mysql_free_result(result);
	}

	return 0;
}

int DbManager::GetUsersBegin(){
	if(transection()==1){
		return DB_BUSY;
	}
	set_transection(1);
	int ret=mysql_query(conn,"select * from tb_user;");
	if(ret){
		set_transection(0);
		return DB_QUERY_FAIL;
	}

	result = mysql_use_result(conn);
	return SUCCESS;
}

int DbManager::GetUsersOneByOne(UserInfo* user){
	if(result){
		row = mysql_fetch_row(result);
		if(row==NULL){
			return DB_NO_MORE_DATA;
		}
		//row[0]:user_id
		//row[1]:user_info,protobuf
		ssp::UserInfoBase pb_user;
		char user_info[10240];
		int len=strlen(row[1]);
		for(int i=0;i<len/2;i++){
			user_info[i]=(row[1][2*i]-'a')*16+(row[1][2*i+1]-'a');
		}
		int ret=pb_user.ParseFromArray(user_info,10240);
		//printf("%s %d\n",row[1],ret);
		user->FromPb(pb_user);
	}else{
		return DB_NO_MORE_DATA;
	}
	return SUCCESS;
}

int DbManager::GetUsersEnd(){
	mysql_free_result(result);
	set_transection(0);

	return 0;
}
int DbManager::GetCurUserId(){
	int user_id=10001;
	int ret=mysql_query(conn,"select value from tb_var where key_str='user_id';");
	if(ret){
		printf("query failed with cur_user_id: %d\n",ret);
		return user_id;
	}else{
		result = mysql_use_result(conn);
		if(result){
			row = mysql_fetch_row(result);
			if(row == NULL){
				printf("result is null with cur_user_id\n");
				return user_id;
			}
			sscanf(row[0],"%d",&user_id);
		}
		mysql_free_result(result);
	}
	return user_id;
}
int DbManager::InsertUser(UserInfo* user){
	ssp::UserInfoBase pb_user;
	user->ToPb(pb_user);
	//printf("user    password: %s\n",user->password());
	//printf("pb_user password: %s\n",pb_user.password().c_str());
	char user_id[256];
	sprintf(user_id,"%d",user->user_id());
	char user_info[10240];
	pb_user.SerializeToArray(user_info,10240);
	char user_info2[10240];
	for(int i=0;i<pb_user.ByteSize();i++){
		int l=user_info[i]&0x000f;
		int h=(user_info[i]&0x00f0)>>4;
		user_info2[i*2]=h+'a';
		user_info2[i*2+1]=l+'a';
	}
	string insertSql="insert into tb_user values ('";
	insertSql+=user_id;
	insertSql+="','";
	insertSql+=user_info2;
	insertSql+="')";
	//printf("insert: %s\n",insertSql.c_str());
	int ret=mysql_query(conn,insertSql.c_str());
	if(ret==0){
		return SUCCESS;
	}else{
		printf("insert user failed : %d\n",ret);
		return DB_QUERY_FAIL;
	}
	return SUCCESS;
}