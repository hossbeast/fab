#ifndef _CANON_H
#define _CANON_H

#include <stdint.h>

#define restrict __restrict

#define CAN_SLASH					0x00			// resolve extra "/" characters
#define CAN_INIT_DOT			0x01			// resolve references to the "." and ".." directories at the initial component 
#define CAN_NEXT_DOT			0x02			// resolve references to the "." and ".." directories at positions other than the initial component 
#define CAN_SYMABS				0x04			// resolve absolute symlinks which do not cross mount points
#define CAN_SYMABSMNT			0x08			// resolve absolute symlinks which cross mount points
#define CAN_SYMREL				0x10			// resolve relative symlinks which do not cross mount points
#define CAN_SYMRELMNT			0x20			// resolve relative symlinks which cross mount points
#define CAN_FORCE_DOT			0x40			// ensure that path begins with '.' - force base to be prepended

#define CAN_REALPATH			0xFF			// canon = realpath (except ENOENT is not an error)

/// canon - man 3 realpath
//
// SUMMARY
//  convert a path into a more canonical representation by performing a subset
//  of path translation operations. when all operations are specified, the result
//  is a canonicalized absolute pathname, as returned by the realpath function
//
// PARAMETERS
//  path				- path to convert
//  pathl       - length of path, 0 for strlen
//  resolved	  - resolved path is written here, but no more than sz bytes are written
//  sz          - size of resolved
//  base        - used to resolve initial references to "." and ".."
//  opts        - bitwise mask of operations to perform - 0 = CAN_REALPATH
//
// RETURNS
//  0 on ENOMEM, 1 otherwise
//
int canon(
	  const char * restrict path
	, int pathl
	, char * const restrict resolved
	, const size_t sz
	, const char * const restrict base
	, const uint32_t opts
)
	__attribute((nonnull));

#undef restrict
#endif
