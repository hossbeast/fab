/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

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

#ifndef _LOGGER_CONFIG_H
#define _LOGGER_CONFIG_H

/// g_logger_default_stderr
//
// SUMMARY
//  changes the default request handling from drop silently to write to stderr
//
// REMARKS
//  an application using liblogger should export this and set it to nonzero
//
int g_logger_default_stderr;

#define restrict __restrict

/// logger_set_process_name
//
// SUMMARY
//  for L_PROCESSID, provide the process name applied to all logs emitted by this process
//
void logger_set_process_name(const char * const restrict name)
  __attribute__((nonnull));

/// logger_set_thread_name
//
// SUMMARY
//  for L_PROCESSID, provide the thread name applied to all logs emitted by this thread
//
void logger_set_thread_name(const char * const restrict name)
  __attribute__((nonnull));

/// logger_set_process_categories
//
// SUMMARY
//  provide a mask of category ids to be applied to all logs emitted by this process
//
void logger_set_process_categories(uint64_t ids);

/// logger_set_thread_categories
//
// SUMMARY
//  provide a mask of category ids to be applied to all logs emitted by this thread 
//
void logger_set_thread_categories(uint64_t ids);

#undef restrict
#endif
