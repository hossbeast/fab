/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.
   
   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef _LISTWISE_LOG_H
#define _LISTWISE_LOG_H

extern struct listwise_logging * listwise_logging_config;

#if DEBUG || DEVEL || SANITY

#define WOULD(type) listwise_logging_config->type ## _would
#define TOKEN(type) listwise_logging_config->type ## _token
#define LOG(type)   listwise_logging_config->type ## _log

#define LW_WOULD(type, udata)																															\
	(	   listwise_logging_config	\
		&& WOULD(type) \
		&& WOULD(type)(TOKEN(type), udata)	\
	)
#define LW_LOG(type, udata, func, file, line, fmt, ...)														\
do {																																							\
	if(LW_WOULD(type, udata))																												\
	{																																								\
		LOG(type)(TOKEN(type), udata, func, file, line, fmt, ##__VA_ARGS__);	\
	}																																								\
} while(0)
#else
#define LW_LOG(type, udata, func, file, line, fmt, ...)
#define LW_WOULD(type, udata) 0
#endif

#define lw_log_generator(fmt, ...)		LW_LOG(generator  , udata, __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define lw_log_exec(fmt, ...)  				LW_LOG(exec  			, udata, __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define lw_log_lstack(fmt, ...)  			LW_LOG(lstack  		, udata, __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define lw_log_opinfo(fmt, ...)				LW_LOG(opinfo			, udata, __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define lw_log_tokens(fmt, ...)				LW_LOG(tokens			, udata, __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define lw_log_states(fmt, ...)				LW_LOG(states			, udata, __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define lw_log_sanity(fmt, ...)				LW_LOG(sanity			, udata, __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define lw_would_generator()	LW_WOULD(generator, udata)
#define lw_would_exec()   		LW_WOULD(exec, udata)
#define lw_would_lstack()   	LW_WOULD(lstack, udata)
#define lw_would_opinfo() 		LW_WOULD(opinfo, udata)
#define lw_would_tokens() 		LW_WOULD(tokens, udata)
#define lw_would_states() 		LW_WOULD(states, udata)
#define lw_would_sanity() 		LW_WOULD(sanity, udata)

#endif
