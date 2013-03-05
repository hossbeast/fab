#ifndef _IDX_H
#define _IDX_H

//
// unfortunately, the word "index" is reserved; its the name of an legacy BSD substr function
//

#include <sys/types.h>
#include <stdint.h>

#define PACK __attribute__((packed))

// index type
struct idx;
typedef struct idx
{
  char*     coll;     // the collection being indexed

  size_t    ewid;     // size of elements in the collection
  size_t    foff;     // offset to the indexed field
  size_t    fwid;     // size of the indexed field
  uint32_t  flags;    // flags

  int       len;      // number of indexed collection elements, elements in locs
  uint16_t  locs[];
} idx;

//
// KINDS OF INDEXES
//
// within an index, if duplicate values are detected:
//
//  FIRST   - discard entries beyond the first
//  EXCLUDE - all non-unique entries discarded
//  UNIQUE  - failure case
//  MULTI   - store all entries
//

#define INDEX_FIRST     0x001
#define INDEX_EXCLUDE   0x002
#define INDEX_UNIQUE    0x003
#define INDEX_MULTI     0x004

//
// TYPES OF INDEXED FIELDS
//
// type of indexed field. its size information, if any, is given in idx.fwid
//
//  NUMERIC - numeric field. size is given in idx.fwid
//   STRING - null-terminated string field
//   V6_SEG - indexed field is an v6_seg (see below)
//

#define INDEX_NUMERIC   0x010
#define INDEX_STRING    0x020
#define INDEX_V6_SEG    0x030

struct v6_seg;
typedef struct v6_seg
{
  uint32_t  base[4];		// base address
  uint32_t  mask[4];		// segment mask
} PACK v6_seg;

//
// ADDITIONAL OPTIONS
//
//  DEREF    - indicates that the collection is a doubly-allocated list, and that
//             therefore, collection elements must be dereferenced
//
//  SKIPZERO - indicates that entries which are "zero" on the indexed field, are
//             not indexed. "zero" means: NUMERIC - zero, STRING - "", V6_SEG - ::
//

#define INDEX_DEREF     		0x100
#define INDEX_SKIPZERO			0x200

//
// macros used to interrogate index flags
//
#define INDEX_KIND      0x00F
#define INDEX_TYPE      0x0F0
#define INDEX_OPTS      0xF00

/// idx_mkindex
//
// specify a field to be indexed. no two elements in the collection may share a value
// on the indexed field. type specifies how such occurences are to be resolved
//
// for numeric fields, don't mix signed with unsigned (they are compared with memcmp)
//
// if *i is nonzero, memory associated with previous index is first free'd
//
// parameters
//
//    coll - collection to be indexed
//    elen - number of elements in the collection
//    ewid - size in bytes of elements in the collection
//    foff - byte offset from start of collection member to the indexed field
//    fwid - size in bytes of the indexed field 
//   flags - one of INDEX_*
//       i - receive the created index
//
int idx_mkindex(void* coll, size_t elen, size_t ewid, size_t foff, size_t fwid, uint32_t flags, idx** i);

/// idx_lookup
//
// _val - get a pointer to en element of a collection by key
// _idx - get the index in the collection of an element by key
//
// parameters
//   coll - collection to search
//    idx - see idx_mkindex
//    key - pointer to key for lookup
//        - INDEX_NUMERIC- pointer to integer
//                           EXAMPLE
//                         uint32_t key = 15;
//                         idx_lookup(i, &key);
//
//        - INDEX_V6_SEG - 128 bytes, a v6 adddress (i.e. uint32_t[4]) even though
//                         the values indexed in the collection are v6_seg's
//                           EXAMPLE
//                         uint32_t addr[4];
//                         idx_lookup(i, &addr);
//
//        - INDEX_STRING - pointer to null-terminated char*
//                           EXAMPLE
//                         char* key = "foo";
//                         idx_lookup(i, &key);
//
//  key_l - size of key
//        - INDEX_NUMERIC- ignored
//        - INDEX_V6_SEG - ignored
//        - INDEX_STRING - string length, or 0 for strlen
//
// _val - returns a pointer to the element, or zero if it was not found
// _idx - returns index of an element, or -1 if it was not found
//
void* idx_lookup_val(idx* i, void* key, int key_l);
int   idx_lookup_idx(idx* i, void* key, int key_l);

/// idx_enumerate
//
// get an array of pointers to collection elements which are sorted on the given index
//
// *list is allocated here, and should be freed by the caller
//
// parameters
//   coll - collection to search
//    idx - see idx_mkindex
//   list - *list is allocated and should be freed by the caller (list is void***)
//    len - *len set to number of elements in *list
//
int idx_enumerate(idx* i, void* list, int* len);

/// idx_sortby
//
// sort the collection on the index. for index types where some collection elements
// may not be indexed (FIRST, EXCLUDE), non-indexed collection elements will appear at the
// end of the collection in an unspecified order
//
void idx_sortby(idx* i);

/// idx_free
//
// free memory associated with the index
//
void idx_free(idx* i);

void idx_xfree(idx** i)
	__attribute__((nonnull));

#endif
