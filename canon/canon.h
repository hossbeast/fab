#ifndef _CANON_H
#define _CANON_H

#include <stdint.h>

#define restrict __restrict

#define CAN_SLASH					0x00			// resolve extra "/" characters
#define CAN_INIT_DOT			0x01			// resolve references to the "." and ".." directories at the initial component 
#define CAN_NEXT_DOT			0x02			// resolve references to the "." and ".." directories at positions other than the initial component 
#define CAN_FORCE_DOT			0x04			// force the path to begin with "."
#define CAN_NEXT_SYM			0x08			// resolve symlinks at positions other than the final component which do not cross mount points
#define CAN_NEXT_SYMMNT		0x10			// resolve symlinks at positions other than the final component which cross mount points
#define CAN_FINL_SYM			0x20			// resolve symlinks at the final component which do not cross mount points
#define CAN_FINL_SYMMNT		0x40			// resolve relative at the final component which cross mount points

#define CAN_REALPATH			0xFF			// canon = realpath (except ENOENT is not an error)

/// canon - man 3 realpath
//
// SUMMARY
//  convert a path into a more canonical representation by performing a subset
//  of path translation operations. when all operations are specified, the result
//  is a canonicalized absolute pathname, as returned by the realpath function (except
//  that ENOENT is not an error)
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
