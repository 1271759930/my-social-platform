/*** 
 * @Author: LZH
 * @Date: 2022-05-26 19:25:47
 * @LastEditTime: 2022-05-26 19:56:26
 * @Description: 
 * @FilePath: /MyFiles/99_my-social-platform/sspdev/common/ret_value.h
 */

#ifndef _RET_VALUE_H
#define _RET_VALUE_H

#define SUCCESS 0

#define USER_NOT_EXIST 100
#define USER_EXIST 101
#define WRONG_PASSWORD 102
#define USER_TOO_MUCH 103

#define NOT_FRIEND 200
#define ALREADY_FRIEND 201
#define FRIEND_TO_MUCH 202
#define FRIEND_INDEX_WRONG 203
#define RELATION_EXIST 204

#define NOT_BLACK 211

#define RELATION_TO_MUCH 220
#define RELATION_NOT_EXIST 221

#define MESSAGE_NOT_EXIST 300
#define MESSAGE_TOO_MUCH 301

#define PHOTO_NOT_EXIST 400
#define PHOTO_TOO_MUCH 401
#define PHOTO_EXIST 402

#define FLAG_INSERT 501
#define FLAG_DELETE 502
#define FLAG_UPDATE 503

#define DB_CONN_INIT_FAIL 601
#define DB_CONN_CONNECT_FAIL 602
#define DB_QUERY_FAIL 603
#define DB_BUSY 604
#define DB_NO_MORE_DATA 605

#endif
