/*** 
 * @Author: LZH
 * @Date: 2022-05-26 19:25:47
 * @LastEditTime: 2022-05-26 19:28:45
 * @Description: 
 * @FilePath: /MyFiles/99_my-social-platform/sspdev/common/macro.h
 */

#ifndef _MACRO_H
#define _MACRO_H

#define GETSETVAR(type, name) \
		private: \
			type  name##_; \
		public: \
			const type& name() const { \
				return name##_; \
			} \
			void set_##name(const type& value) { \
				name##_ = value; \
			} \
		private:

#define GETSETSTR(size, name) \
		private: \
			char name##_[size]; \
		public: \
		const char* name() const { \
			return name##_; \
		} \
		void set_##name(const char *value) {\
			strncpy(name##_,value,size);\
		} \
		private:

#endif
