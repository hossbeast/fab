#ifndef _DIRUTIL_H
#define _DIRUTIL_H

#include <sys/types.h>

#define restrict __restrict

/// rmdir_recursive
//
// SUMMARY
//  recursively delete a directory and all of its contents, which consists only
//  of files and directories
//
// PARAMETERS
//  dirpath - 
//  rmself  - if true, delete directory itself as well
//
int rmdir_recursive(const char * const restrict dirpath, int rmself)
	__attribute__((nonnull));

/// mkdirp
//
// SUMMARY
//  ensure a directory path exists, iteratively creating each component of the path
//  if it does not already exist
//
// PARAMETERS
//  dirpath - path to directory to create
//  mode    - mode to create directories with
// 
// RETURNS
//  0 if an error OTHER THAN EEXIST was encountered, 1 otherwise
//
int mkdirp(const char * const restrict dirpath, mode_t mode)
	__attribute__((nonnull));

#undef restrict
#endif
