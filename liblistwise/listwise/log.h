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
#define LW_WOULD(cb, udata) (listwise_logging_config && listwise_logging_config->cb && udata)
#define LW_LOG(cb, udata, func, file, line, fmt, ...)															\
do {																																							\
	if(LW_WOULD(cb, udata))																													\
	{																																								\
		listwise_logging_config->cb((*udata), func, file, line, fmt, ##__VA_ARGS__);	\
	}																																								\
} while(0)
#else
#define LW_LOG(cb, udata, func, file, line, fmt, ...)
#define LW_WOULD(cb, udata) 0
#endif

#define lw_log_dump(fmt, ...)   LW_LOG(log_dump  , udata, __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define lw_log_opinfo(fmt, ...) LW_LOG(log_opinfo, udata, __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define lw_log_tokens(fmt, ...) LW_LOG(log_tokens, udata, __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define lw_log_states(fmt, ...) LW_LOG(log_states, udata, __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define lw_log_sanity(fmt, ...) LW_LOG(log_sanity, udata, __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define lw_would_dump()   LW_WOULD(log_dump  , udata)
#define lw_would_opinfo() LW_WOULD(log_opinfo, udata)
#define lw_would_tokens() LW_WOULD(log_tokens, udata)
#define lw_would_states() LW_WOULD(log_states, udata)
#define lw_would_sanity() LW_WOULD(log_sanity, udata)

#endif
