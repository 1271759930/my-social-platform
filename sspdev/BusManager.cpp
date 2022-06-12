/*** 
 * @Author: LZH
 * @Date: 2022-05-26 19:25:46
 * @LastEditTime: 2022-06-12 20:50:50
 * @Description: 
 * @FilePath: /MyFiles/99_my-social-platform/sspdev/BusManager.cpp
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BusManager.h"

int debug=1;

#define SIZE_BUS_FLAG 16
#define SIZE_CHANNEL sizeof(Channel)
#define SIZE_CHANNEL_NAME 256
#define SIZE_CHANNEL_FROM sizeof(int)
#define SIZE_CHANNEL_TO sizeof(int)
#define SIZE_BLOCK sizeof(BusBlock)
#define SIZE_BLOCK_NEXT sizeof(BusBlock*)
#define SIZE_BLOCK_USED 8
#define SIZE_BLOCK_SIZE sizeof(int)
#define SIZE_BLOCK_DATA 10240

#define OFFSET_BUS_FLAG 0
#define OFFSET_CHANNEL 0
#define OFFSET_CHANNEL_NAME OFFSET_CHANNEL+SIZE_CHANNEL
#define OFFSET_CHANNEL_FROM OFFSET_CHANNEL_NAME+SIZE_CHANNEL_NAME
#define OFFSET_CHANNEL_TO OFFSET_CHANNEL_FROM+SIZE_CHANNEL_FROM
#define OFFSET_CHANNEL_BLOCK OFFSET_CHANNEL_TO+SIZE_CHANNEL_TO
#define OFFSET_CHANNEL_BLOCK_NEXT OFFSET_CHANNEL_BLOCK+SIZE_BLOCK
#define OFFSET_CHANNEL_BLOCK_USED OFFSET_CHANNEL_BLOCK_NEXT+SIZE_BLOCK_NEXT
#define OFFSET_CHANNEL_BLOCK_SIZE OFFSET_CHANNEL_BLOCK_USED+SIZE_BLOCK_USED
#define OFFSET_CHANNEL_BLOCK_DATA OFFSET_CHANNEL_BLOCK_SIZE+SIZE_BLOCK_SIZE

#define UNIT_SIZE (OFFSET_CHANNEL_BLOCK_DATA+SIZE_BLOCK_DATA)

int BusManager::Init(){
	printf("----  Init Shm  ----\n");

	shm_id=shmget((key_t)8998,102400,IPC_CREAT | 0666);
	if(shm_id<0){
		printf("shmget failed : %d\n",shm_id);
		return 0;
	}
	printf("shm_id=%d\n",shm_id);
	shmp = (char*)shmat(shm_id,NULL,0);
	if(*shmp<0){
		printf("shmat failed: %d\n",*shmp);
		return 0;
	}
	printf("shmp_addr=%p\n",shmp);
	if(*shmp==0){
		*shmp='Y';
		BusInit();
		ChannelInit();
		printf("----  After All  ----\n");
		printf("ipcrm -m %d\n",shm_id);
	}else{
		// ---- Bus Build ----
	}
	printf("----  Init Finish!  ----\n");

	return 0;
}

int BusManager::BusInit(){
	printf("----  Init Bus  ----\n");

	char* cur_addr=shmp+SIZE_BUS_FLAG;
	
	if(debug)printf("cur_addr:%p\n",cur_addr);
	channel_[0]=(Channel*)cur_addr;
	cur_addr+=SIZE_CHANNEL;
	if(debug)printf("channel_[0]:%p\n",channel_[0]);
	
	/* ---- channel0 ----*/
	if(debug)printf("cur_addr:%p\n",cur_addr);
	channel_[0]->name=cur_addr;
	cur_addr+=SIZE_CHANNEL_NAME;
	if(debug)printf("channel_[0]->name:%p\n",channel_[0]->name);

	if(debug)printf("cur_addr:%p\n",cur_addr);
	(*channel_[0]).from=(int*)cur_addr;
	cur_addr+=SIZE_CHANNEL_FROM;
	if(debug)printf("channel_[0]->from:%p\n",channel_[0]->from);
	
	if(debug)printf("cur_addr:%p\n",cur_addr);
	channel_[0]->to=(int*)cur_addr;
	cur_addr+=SIZE_CHANNEL_TO;
	if(debug)printf("channel_[0]->to:%p\n",channel_[0]->to);
	
	if(debug)printf("cur_addr:%p\n",cur_addr);
	channel_[0]->block=(BusBlock*)cur_addr;
	cur_addr+=SIZE_BLOCK;
	if(debug)printf("channel_[0]->block:%p\n",channel_[0]->block);

	if(debug)printf("cur_addr:%p\n",cur_addr);
	channel_[0]->block->next=(BusBlock*)cur_addr;	
	cur_addr+=SIZE_BLOCK_NEXT;
	if(debug)printf("channel_[0]->block->next:%p\n",channel_[0]->block->next);

	if(debug)printf("cur_addr:%p\n",cur_addr);
	channel_[0]->block->used=cur_addr;
	cur_addr+=SIZE_BLOCK_USED;
	if(debug)printf("channel_[0]->block->used:%p\n",channel_[0]->block->used);

	if(debug)printf("cur_addr:%p\n",cur_addr);
	channel_[0]->block->size=(int*)cur_addr;
	cur_addr+=SIZE_BLOCK_SIZE;
	if(debug)printf("channel_[0]->block->size:%p\n",channel_[0]->block->size);

	if(debug)printf("cur_addr:%p\n",cur_addr);
	channel_[0]->block->data=cur_addr;
	cur_addr+=SIZE_BLOCK_DATA;
	if(debug)printf("channel_[0]->block->data:%p\n",channel_[0]->block->data);

	/* ---- channel1 ~ channel3 ----*/
	for(int i=1;i<4;i++){
		if(debug)printf("cur_addr:%p\n",cur_addr);
		channel_[i]=(Channel*)cur_addr;
		cur_addr+=SIZE_CHANNEL;
		if(debug)printf("channel_[%d]:%p\n",i,channel_[i]);

		if(debug)printf("cur_addr:%p\n",cur_addr);
		channel_[i]->name=cur_addr;
		cur_addr+=SIZE_CHANNEL_NAME;
		if(debug)printf("channel_[%d]->name:%p\n",i,channel_[i]->name);

		if(debug)printf("cur_addr:%p\n",cur_addr);
		channel_[i]->from=(int*)cur_addr;
		cur_addr+=SIZE_CHANNEL_FROM;
		if(debug)printf("channel_[%d]->from:%p\n",i,channel_[i]->from);
		
		if(debug)printf("cur_addr:%p\n",cur_addr);
		channel_[i]->to=(int*)cur_addr;
		cur_addr+=SIZE_CHANNEL_TO;
		if(debug)printf("channel_[%d]->to:%p\n",i,channel_[i]->to);
		
		if(debug)printf("cur_addr:%p\n",cur_addr);
		channel_[i]->block=(BusBlock*)cur_addr;
		cur_addr+=SIZE_BLOCK;
		if(debug)printf("channel_[%d]->block:%p\n",i,channel_[i]->block);
		
		if(debug)printf("cur_addr:%p\n",cur_addr);
		channel_[i]->block->next=(BusBlock*)cur_addr;	
		cur_addr+=SIZE_BLOCK_NEXT;
		if(debug)printf("channel_[%d]->block->next:%p\n",i,channel_[i]->block->next);

		if(debug)printf("cur_addr:%p\n",cur_addr);
		channel_[i]->block->used=cur_addr;
		cur_addr+=SIZE_BLOCK_USED;
		if(debug)printf("channel_[%d]->block->used:%p\n",i,channel_[i]->block->used);

		if(debug)printf("cur_addr:%p\n",cur_addr);
		channel_[i]->block->size=(int*)cur_addr;
		cur_addr+=SIZE_BLOCK_SIZE;
		if(debug)printf("channel_[%d]->block->size:%p\n",i,channel_[i]->block->size);

		if(debug)printf("cur_addr:%p\n",cur_addr);
		channel_[i]->block->data=cur_addr;
		cur_addr+=SIZE_BLOCK_DATA;
		if(debug)printf("channel_[%d]->block->data:%p\n",i,channel_[i]->block->data);

	}
	return 0;
}

int BusManager::ChannelInit(){
	printf("----  Channel Init ----\n");

	for(int i=0;i<4;i++){
		memset(MutableChannelName(i),0,SIZE_CHANNEL_NAME);
		*MutableChannelBlockUsed(i)='N';
		*MutableChannelBlockSize(i)=0;
		memset(MutableChannelBlockData(i),0,SIZE_BLOCK_DATA);
		
	}
	strncpy(MutableChannelName(0),"Usr1->Server",12);
	*MutableChannelFrom(0)=11;
	*MutableChannelTo(0)=21;

	strncpy(MutableChannelName(1),"Usr2->Server",12);
	*MutableChannelFrom(1)=12;
	*MutableChannelTo(1)=21;

	strncpy(MutableChannelName(2),"Server->Usr1",12);
	*MutableChannelFrom(2)=21;
	*MutableChannelTo(2)=11;

	strncpy(MutableChannelName(3),"Server->Usr2",12);
	*MutableChannelFrom(3)=21;
	*MutableChannelTo(3)=12;

	ChannelShow();
	return 0;
}

int BusManager::ChannelShow(){
	printf("----  Channel Display  ----\n");
	for(int i=0;i<4;i++){
		printf("Channel[%d] Name: %s\n",i,MutableChannelName(i));
		printf("Channel[%d] From: %d\n",i,*MutableChannelFrom(i));
		printf("Channel[%d] To  : %d\n",i,*MutableChannelTo(i));
		printf("Channel[%d] Used: %c\n",i,*MutableChannelBlockUsed(i));
		printf("Channel[%d] Size: %d\n",i,*MutableChannelBlockSize(i));
		printf("Channel[%d] Data: %s\n",i,MutableChannelBlockData(i));
	}
	return 0;
}

int BusManager::ChannelQuickShow(){
	printf("----  Channel Quick Show  ----\n");
	for(int i=0;i<4;i++){
		printf("Channel[%d] Name: %s ",i,MutableChannelName(i));
		printf("Used: %c ",*MutableChannelBlockUsed(i));
		printf("Size: %d\n",*MutableChannelBlockSize(i));
	}
	return 0;
}

char* BusManager::MutableChannelName(int i){
	int offset_in=OFFSET_CHANNEL_NAME;
	int offset_out=i*UNIT_SIZE;
	return (shmp+offset_in+offset_out+SIZE_BUS_FLAG);
}

int* BusManager::MutableChannelFrom(int i){
	int offset_in=OFFSET_CHANNEL_FROM;
	int offset_out=i*UNIT_SIZE;
	return (int*)(shmp+offset_in+offset_out+SIZE_BUS_FLAG);
}

int* BusManager::MutableChannelTo(int i){
	int offset_in=OFFSET_CHANNEL_TO;
	int offset_out=i*UNIT_SIZE;
	return (int*)(shmp+offset_in+offset_out+SIZE_BUS_FLAG);
}

int BusManager::MutableChannelBlock(int i){
	int offset_in=OFFSET_CHANNEL_BLOCK;
	int offset_out=i*UNIT_SIZE;
	return *((int*)shmp+offset_in+offset_out+SIZE_BUS_FLAG);
}

BusBlock* BusManager::MutableChannelBlockNext(int i){
	int offset_in=OFFSET_CHANNEL_BLOCK_NEXT;
	int offset_out=i*UNIT_SIZE;
	return (BusBlock*)(shmp+offset_in+offset_out+SIZE_BUS_FLAG);
}

char* BusManager::MutableChannelBlockUsed(int i){
	int offset_in=OFFSET_CHANNEL_BLOCK_USED;
	int offset_out=i*UNIT_SIZE;
	return (char*)(shmp+offset_in+offset_out+SIZE_BUS_FLAG);
}

int* BusManager::MutableChannelBlockSize(int i){
	int offset_in=OFFSET_CHANNEL_BLOCK_SIZE;
	int offset_out=i*UNIT_SIZE;
	return (int*)(shmp+offset_in+offset_out+SIZE_BUS_FLAG);
}

char* BusManager::MutableChannelBlockData(int i){
	int offset_in=OFFSET_CHANNEL_BLOCK_DATA;
	int offset_out=i*UNIT_SIZE;
	return (shmp+offset_in+offset_out+SIZE_BUS_FLAG);
}

int BusManager::GetChannelIndex(int from,int to){
	for(int i=0;i<4;i++){
		if(*MutableChannelTo(i)==to){
			if(*MutableChannelFrom(i)==from){
				return i;
			}
		}
	}
	return -1;
}

char* BusManager::GetChannelName(int from,int to){
	int idx=GetChannelIndex(from,to);
	if(idx!=-1){
		return MutableChannelName(idx);
	}
	return NULL;
}
char* BusManager::Recv(int from,int to){
	int idx=GetChannelIndex(from,to);
	if(idx!=-1){
		if(*MutableChannelBlockUsed(idx)=='Y'){
			*MutableChannelBlockUsed(idx)='N';
			return MutableChannelBlockData(idx);
		}
	}
	return NULL;
}

int BusManager::CheckRecv(int target_id){
	for(int i=0;i<4;i++){
		if(*MutableChannelTo(i)==target_id){
			if(*MutableChannelBlockUsed(i)=='Y'){
				printf("----[debg]channel %s recv %d bytes.\n",MutableChannelName(i),*MutableChannelBlockSize(i));
				return *MutableChannelFrom(i);			
			}
		}
	}
	return -1;
}

int BusManager::RecvSize(int from, int to){
	int idx=GetChannelIndex(from,to);
	if(idx!=-1){
		return *MutableChannelBlockSize(idx);
	}
	return 0;
}

int BusManager::ChannelsClear(){
	for(int i=0;i<4;i++){
		int size=*MutableChannelBlockSize(i);
		*MutableChannelBlockUsed(i)='N';
		*MutableChannelBlockSize(i)=0;
		memset(MutableChannelBlockData(i),0,size);
	}
	return 0;
}

int BusManager::Clear(int from,int to){
	int idx=GetChannelIndex(from,to);
	if(idx!=-1){
		int size=*MutableChannelBlockSize(idx);
		*MutableChannelBlockUsed(idx)='N';
		*MutableChannelBlockSize(idx)=0;
		memset(MutableChannelBlockData(idx),0,size);
	}
	return 0;
}

int BusManager::Send(int from,int to,char *data,int size){
	int idx=GetChannelIndex(from,to);
	if(idx==-1){
		printf("----\033[31m[erro]Channel(%d,%d) Not Exist\033[0m\n",from,to);
		return -2;
	}
	char* p_ch_used=MutableChannelBlockUsed(idx);
	int* p_ch_size=MutableChannelBlockSize(idx);

	if(*p_ch_used!='N'){
		printf("----[warn]Channel(%s) Is Busy\n",MutableChannelName(idx));
		return -3;
	}
	if(idx!=-1&&*p_ch_used=='N'){
		strncpy(MutableChannelBlockData(idx),data,size);
		*p_ch_size=size;
		*p_ch_used='Y';
		return 0;
	}
	return -1;
}

int BusManager::Detach(){
	int ret=shmdt(shmp);
	if(ret == 0){
		printf("----[info]shmdt ok\n");
	}else{
		printf("----[erro]shmdt failed: %d\n",ret);
	}
	return 0;
}

int BusManager::Remove(){
	int ret=shmctl(shm_id, IPC_RMID, 0);
	if(ret == 0){
		printf("----[info]shmctl ok\n");
	}else{
		printf("----[erro]shmctl failed: %d\n",ret);
	}
	return 0;
}

