#ifndef _CONTROL_H
#define _CONTROL_H

#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "log.h"

#define fail(fmt, ...)									\
	do {																	\
		log(L_ERROR, fmt " at [%s:%d (%s)]"	\
			, ##__VA_ARGS__										\
			, __FILE__												\
			, __LINE__												\
			, __FUNCTION__										\
		);																	\
		return 0;														\
	} while(0)
 
#define fatal(x, ...)														\
	do {																					\
		if((x(__VA_ARGS__)) == 0)										\
		{																						\
			log(L_ERROR, #x " failed at [%s:%d (%s)]"	\
				, __FILE__															\
				, __LINE__															\
				, __FUNCTION__													\
			);																				\
			return 0;																	\
		}																						\
	} while(0)

#define fatal_os(x, ...)																				\
	do {																													\
		if((x(__VA_ARGS__)) != 0)																		\
		{																														\
			log(L_ERROR, #x " failed with: [%d][%s] at [%s:%d (%s)]"	\
				, errno																									\
				, strerror(errno)																				\
				, __FILE__																							\
				, __LINE__																							\
				, __FUNCTION__																					\
			);																												\
			return 0;																									\
		}																														\
	} while(0)

#endif
