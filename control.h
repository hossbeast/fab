#ifndef _CONTROL_H
#define _CONTROL_H

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/// finally
//
//
//
#define finally				\
	int _coda_r;				\
	goto CODA_GOOD;			\
CODA:									\
	_coda_r = 0;				\
	goto CODA_FINALLY;	\
CODA_GOOD:						\
	_coda_r = 1;				\
CODA_FINALLY

/// coda
//
//
//
#define coda return _coda_r

/// error
//
// SUMMARY
//  log an L_ERROR message
//
#define error(fmt, ...)																\
	do {																								\
		dprintf(listwise_err_fd, fmt " at [%s:%d (%s)]\n"	\
			, ##__VA_ARGS__																	\
			, __FILE__																			\
			, __LINE__																			\
			, __FUNCTION__																	\
		);																								\
	} while(0)

/// fail
//
// SUMMARY
//  log an L_ERROR message, terminate the current scope with failure
//
#define fail(fmt, ...)							\
	do {															\
		error(fmt "\n", ##__VA_ARGS__);	\
		goto CODA;											\
	} while(0)

/// qfail
//
// SUMMARY
//  silently terminate the current scope with failure
//
#define qfail()	goto CODA

/// qterm
//
// SUMMARY
//  silently terminate the current scope with success
//
#define qterm() goto CODA_GOOD

/// fatal
//
// SUMMARY
//  execute the specified function with zero-return-failure semantics and if it fails
//  log an L_ERROR message and terminate the current scope with failure
//
#define fatal(x, ...)														\
	do {																					\
		if((x(__VA_ARGS__)) == 0)										\
		{																						\
			dprintf(listwise_err_fd, #x " failed at [%s:%d (%s)]"	\
				, __FILE__															\
				, __LINE__															\
				, __FUNCTION__													\
			);																				\
			goto CODA;																\
		}																						\
	} while(0)
 
/// qfatal
//
// SUMMARY
//  execute the specified function with zero-return-failure semantics and if it fails
//  silently terminate the current scope with failure
//
#define qfatal(x, ...)													\
	do {																					\
		if((x(__VA_ARGS__)) == 0)										\
		{																						\
			goto CODA;																\
		}																						\
	} while(0)

/// fatal_os
//
// SUMMARY
//  execute the specified function with nonzero-return-failure semantics and if it fails
//  log an L_ERROR message and terminate the current scope with failure
//
#define fatal_os(x, ...)																				\
	do {																													\
		if((x(__VA_ARGS__)) != 0)																		\
		{																														\
			dprintf(listwise_err_fd, #x " failed with: [%d][%s] at [%s:%d (%s)]"	\
				, errno																									\
				, strerror(errno)																				\
				, __FILE__																							\
				, __LINE__																							\
				, __FUNCTION__																					\
			);																												\
			goto CODA;																								\
		}																														\
	} while(0)

/// fatal_os
//
// SUMMARY
//  execute the specified function with nonzero-return-failure semantics and if it fails
//  silently terminate the current scope with failure
//
#define qfatal_os(x, ...)																				\
	do {																													\
		if((x(__VA_ARGS__)) != 0)																		\
		{																														\
			goto CODA;																								\
		}																														\
	} while(0)

#endif

