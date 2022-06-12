/*** 
 * @Author: LZH
 * @Date: 2022-05-26 19:25:46
 * @LastEditTime: 2022-06-12 21:39:19
 * @Description: 
 * @FilePath: /MyFiles/99_my-social-platform/sspdev/Config.cpp
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Config.h"
int Config::ReadConfig(){
	FILE* f=fopen("config.ini","r");
	if(f==NULL){
		return -1;
	}
	while(1){
		char config_line[256];
		if(fscanf(f,"%s",config_line)==EOF){
			break;
		}
		if(strstr(config_line,"use_socket")!=NULL){
			int used;
			sscanf(config_line,"use_socket=%d",&used);
			if(used==0){
				printf("socket disabled\n");
			}
			use_socket_=used;
		}
		if(strstr(config_line,"socket_port")!=NULL){
			int port;
			sscanf(config_line,"socket_port=%d",&port);
			printf("port:%d\n",port);
			socket_port_=port;
		}
		if(strstr(config_line,"socket_addr")!=NULL){
			char scaddr[256];
			sscanf(config_line,"socket_addr=%s",scaddr);
			strncpy(socket_addr_,scaddr,256);
			printf("addr:%s\n",scaddr);
		}
	}
	fclose(f);
	return 0;
}
