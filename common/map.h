#ifndef _MAP_H

#include <stdint.h>

#define restrict __restrict

/// opaque map type declaration
//
struct map_t;
typedef struct map_t map;

/// map_create
//
// create an empty map
//
// parameters
//  map					- created map goes here
//  destructor  - invoked with key/value just before freeing their associated memory
//
// returns nonzero on success
//
int map_create(map** const restrict m, void (*destructor)(void*, void*))
	__attribute__((nonnull(1)));

/// map_free
//
// free a map with free semantics
//
void map_free(map* const restrict map);

/// map_xfree
//
// free a map with xfree semantics
//
void map_xfree(map** const restrict map)
	__attribute__((nonnull));

/// MM - helper macro
//
// many map api's require an item and its length (get, set, delete)
//
// when the items size is known at compile time, MM makes it simpler to pass
//
#ifndef MAP_NO_HELPERS
#define MM(x) (void*)&(x), sizeof(x)
#define MMS(x) (x), strlen(x)
#endif

/// map_set
//
// set key/value pair on the map
//
// parameters
//  map				- 
//  key				- pointer to key
//  key_len		- key length
//  [value]		- pointer to value, or 0
//  value_len	- value length
//
// returns pointer to the stored value, or 0 on error
//
void* map_set (map* const restrict map, const void* const restrict key, int key_len, const void* const restrict value, int value_len)
	__attribute__((nonnull(1, 2)));

/// map_get
//
// given a key, returns pointer to associated value, or 0 if not found
//
// parameters
//  map				-
//	key				- pointer to key
//	key_len		- length of key
//
// example
//	int k = 15;
//	int* v = 0;
//	if((v = map_get(map, MM(k))) == 0)
//		/* key is not set */
//
// returns pointer to the stored value, or 0 if not found
//
void* map_get(const map* const restrict map, const void* const restrict key, int key_len)
	__attribute__((nonnull));

/// map_clear
//
// disassociate all keys - internal structures in the map remain allocated
//
// returns nonzero on success
//
void map_clear(map* const restrict map)
	__attribute__((nonnull));

/// map_delete
//
// disassociates the specified key - if not found, does nothing
//
// returns 1 if the key was found, 0 otherwise
//
int map_delete(map* const restrict map, const void* const restrict key, int key_len)
	__attribute__((nonnull));

/// map_keys
//
// allocates *target and points *target[0...n] at each of the keys presently set in the map
// and populates *count.
//
// *target is free()'d by the caller
//
// returns nonzero on success
//
int map_keys(const map* const restrict map, void* const restrict target, int* const restrict count)
	__attribute__((nonnull));

/// map_values
//
// allocates *target and points *target[0...n] at each of the values presently associated into
// the map and populates *count
//
// *target is free()'d by the caller
//
// returns nonzero on success
//
int map_values(const map* const restrict map, void* const restrict target, int* const restrict count)
	__attribute__((nonnull));

#undef restrict
#endif
