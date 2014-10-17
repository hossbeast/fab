/* Copyright (c) 2012-2014 Todd Freed <todd.freed@gmail.com>

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

#ifndef _LISTWISE_INTERNAL_H
#define _LISTWISE_INTERNAL_H

#include "xapi.h"

// all externally-visible liblistwise apis
#include "listwise.h"
#include "describe.h"
#include "describe.internal.h"
#include "exec.h"
#include "fs.h"
#include "generator.h"
#include "genscan.h"
#include "iterate.h"
#include "logging.h"
#include "lstack.h"
#include "lwx.h"
#include "object.h"
#include "operator.h"
#include "operator.internal.h"
#include "operators.h"
#include "re.h"
#include "selection.h"
#include "tune.h"
#include "window.h"

// modules from common
#include "coll.h"
#include "map.h"

// error tables
#include "LW.errtab.h"
//#include "PCRE.errtab.h"
#undef perrtab
#define perrtab perrtab_LW

// visibility declarations
#define API __attribute__((visibility("protected")))
#define APIDATA

#define restrict __restrict

// collection of registered object types with lookup index by type id
map * object_registry;

/*
** internal sanity declarations
*/

#if SANITY
typedef struct
{
	struct
	{
		int l;
		int a;
		uint8_t t;

		int x;
		int y;
		typeof(((struct lwx_t*)0)->s[0].s[0].s[0]) *	s;
		typeof(((struct lwx_t*)0)->s[0].s[0].s[0]) *	o;
		int ol;
		int oa;
	} *																	s_strings;
	int																	s_stringsl;
	int																	s_stringsa;
} sanityblock;

int sanityblock_create(sanityblock ** const restrict sb)
	__attribute__((nonnull));

void sanityblock_free(sanityblock * const restrict sb)
	__attribute__((nonnull));

void sanityblock_reset(sanityblock * const restrict sb)
	__attribute__((nonnull));

int sanity(struct lwx_t * const restrict lx, sanityblock * const restrict sb, void ** udata)
	__attribute__((nonnull));
#endif

/*
** internal logging declarations
*/

#define WOULD(type) listwise_logging_config->type ## _would
#define TOKEN(type) listwise_logging_config->type ## _token
#define LOG(type)   listwise_logging_config->type ## _log

#define LW_WOULD(type, udata)									\
	(	   listwise_logging_config								\
		&& WOULD(type) 														\
		&& WOULD(type)(TOKEN(type), udata)				\
	)

#define LW_LOG(type, udata, func, file, line, fmt, ...)														\
do {																																							\
	if(LW_WOULD(type, udata))																												\
	{																																								\
		LOG(type)(TOKEN(type), udata, func, file, line, fmt, ##__VA_ARGS__);					\
	}																																								\
} while(0)

#define lw_log_generator(fmt, ...)		LW_LOG(generator  , udata, __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define lw_log_exec(fmt, ...)  				LW_LOG(exec  			, udata, __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define lw_log_lstack(fmt, ...)  			LW_LOG(lstack  		, udata, __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define lw_log_opinfo(fmt, ...)				LW_LOG(opinfo			, udata, __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define lw_would_generator()					LW_WOULD(generator, udata)
#define lw_would_exec()   						LW_WOULD(exec, udata)
#define lw_would_lstack()   					LW_WOULD(lstack, udata)
#define lw_would_opinfo() 						LW_WOULD(opinfo, udata)

#if DEVEL
#define lw_log_tokens(fmt, ...)				LW_LOG(tokens			, udata, __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define lw_log_states(fmt, ...)				LW_LOG(states			, udata, __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define lw_log_sanity(fmt, ...)				LW_LOG(sanity			, udata, __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define lw_would_tokens() 						LW_WOULD(tokens, udata)
#define lw_would_states() 						LW_WOULD(states, udata)
#define lw_would_sanity() 						LW_WOULD(sanity, udata)
#else
#define lw_log_tokens(fmt, ...)
#define lw_log_states(fmt, ...)
#define lw_log_sanity(fmt, ...)

#define lw_would_tokens() 						0
#define lw_would_states() 						0
#define lw_would_sanity() 						0
#endif

#undef restrict
#endif
