/*** 
 * @Author: LZH
 * @Date: 2022-05-26 19:25:46
 * @LastEditTime: 2022-05-27 15:18:34
 * @Description: 
 * @FilePath: /MyFiles/99_my-social-platform/sspdev/PhotoManager.h
 */

#ifndef _PHOTOMANAGER_H
#define _PHOTOMANAGER_H
#include "PhotoInfo.h"

class PhotoManager {
private:
	PhotoInfo photos_[10240];
	GETSETVAR(int,photo_count)

public:
	void Start();
	void Proc();
	void Shutdown();
	void Restart();

public:
	int CreatePhoto(int user_id);
	PhotoInfo* GetPhoto(int user_id);
	int UpdatePhoto(int user_id,int publisher_id,int publish_time,int publish_message_id);
};

#endif
