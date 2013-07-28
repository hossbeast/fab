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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "xmem.h"
#include "xstring.h"

#include "idx.h"

//
// [[ static functions ]]
//

/// swap
//
// swap the elements of the collection at the given indexes
//
static void swap(idx* i, int a, int b)
{
	size_t z = 0;
	
	if(i->flags & INDEX_OPTS & INDEX_DEREF)
		z = sizeof(void*);
	else
		z = i->ewid;

	char tmp[z];
	memcpy(tmp, i->coll + (z * a), z);
	memcpy(i->coll + (z * a), i->coll + (z * b), z);
	memcpy(i->coll + (z * b), tmp, z);
}

/// cmp
//
// compare, on the indexed field, elements of the collection pointed to
// by the given pointers
//
static int cmp(idx* i, const char* A, int Al, const char* B)
{
	if((i->flags & INDEX_TYPE) == INDEX_NUMERIC)
	{
		return memcmp(A, B, i->fwid);
	}
	else if((i->flags & INDEX_TYPE) == INDEX_STRING)
	{
		if(Al)
			return xstrcmp(*((char**)A), Al, *((char**)B), 0, 0);
		else
			return strcmp(*((char**)A), *((char**)B));
	}
	else if((i->flags & INDEX_TYPE) == INDEX_V6_SEG)
	{
		const v6_seg* a = (void*)A;
		const v6_seg* b = (void*)B;

		return
			   a->mask[0] > b->mask[0] ? -1 : a->mask[0] < b->mask[0] ? 1
			 : a->mask[1] > b->mask[1] ? -1 : a->mask[1] < b->mask[1] ? 1
			 : a->mask[2] > b->mask[2] ? -1 : a->mask[2] < b->mask[2] ? 1
			 : a->mask[3] > b->mask[3] ? -1 : a->mask[3] < b->mask[3] ? 1
			 : a->base[3] < b->base[3] ? -1 : a->base[3] > b->base[3] ? 1 
			 : a->base[2] < b->base[2] ? -1 : a->base[2] > b->base[2] ? 1 
			 : a->base[1] < b->base[1] ? -1 : a->base[1] > b->base[1] ? 1 
			 : a->base[0] < b->base[0] ? -1 : a->base[0] > b->base[0] ? 1
			 : 0;
	}

	return 0;
}

/// iszero
//
// compare, on the indexed field, an element of the collection pointed to
// by the given pointer, to the "zero" value
//
// has comparison semantics, so 0 means equal
//
static int iszero(idx* i, const char* A)
{
	static struct v6_seg		zero6		= { };
	static char*						zeros		= "";

	if((i->flags & INDEX_TYPE) == INDEX_NUMERIC)
	{
		char* zeron = alloca(i->fwid);
		memset(zeron, 0, i->fwid);

		return cmp(i, A, 0, (void*)zeron);
	}
	else if((i->flags & INDEX_TYPE) == INDEX_STRING)
	{
		return cmp(i, A, 0, (void*)&zeros);
	}
	else if((i->flags & INDEX_TYPE) == INDEX_V6_SEG)
	{
		return cmp(i, A, 0, (void*)&zero6);
	}

	return 0;
}

/// elat
//
// get a pointer to the element of the collection at the specified index
//
static void* elat(idx* i, int x)
{
	if(i->flags & INDEX_OPTS & INDEX_DEREF)
	{
		return *((char**)(i->coll + (sizeof(void*) * x))) + i->foff;
	}
	else
	{
		return i->coll + (i->ewid * x) + i->foff;
	}
}

static void* lookup(idx* i, void* key, int key_l)
{
	int keycmp(const void* __attribute__((unused)) k, const void* b)
	{
		const char* B;
		if(i->flags & INDEX_OPTS & INDEX_DEREF)
		{
			B = (*((char**)(i->coll + (sizeof(void*) * (*(uint16_t*)b))))) + i->foff;
		}
		else
		{
			B = i->coll + (i->ewid * (*(uint16_t*)b)) + i->foff;
		}

		return cmp(i, key, key_l, B);
	};

	uint16_t* p = 0;
	if((i->flags & INDEX_TYPE) == INDEX_V6_SEG)
	{
		// cascade-search
		int x;
		for(x = 0; x < i->len; x++)
		{
			const uint32_t*	a = key;
			const v6_seg*		b = elat(i, i->locs[x]);

			int r = 
				  (a[0] & b->mask[0]) == b->base[0]
			 && (a[1] & b->mask[1]) == b->base[1]
			 && (a[2] & b->mask[2]) == b->base[2]
			 && (a[3] & b->mask[3]) == b->base[3];

			if(r)
			{
				p = &i->locs[x];
				break;
			}
		}
	}
	else
	{
		p = (uint16_t*)bsearch(key, i->locs, i->len, sizeof(i->locs[0]), keycmp);
	}

	return p;

	if(p)
	{
		if(i->flags & INDEX_OPTS & INDEX_DEREF)
			return *((void**)(i->coll + (sizeof(void*) * (*p))));
		else
			return i->coll + (i->ewid * (*p));
	}
	
	return 0;
}

/// elcmp
//
// compare, on the indexed field, elements of the collection at the given indexes
//
static int elcmp(idx* i, int a, int b)
{
	return cmp(i, elat(i, a), 0, elat(i, b));
}

//
// [[ API functions ]]
//

int idx_mkindex(void* coll, size_t elen, size_t ewid, size_t foff, size_t fwid, uint32_t flags, idx** i)
{
	// allocate the index with an upper limit on the number of elements in locs
	if(xrealloc(i, sizeof(*i[0]) + (sizeof((*i)->locs[0]) * elen), 1, 0) != 0)
		return 1;

	(*i)->coll = coll;
	(*i)->ewid = ewid;
	(*i)->foff = foff;
	(*i)->fwid = fwid;
	(*i)->flags = flags;

	int x;
	int y;
	int r;

	// linear insertion sort into locs
	for(x = 0; x < elen; x++)
	{
		// check for a zero-valued entry
		if(flags & INDEX_OPTS & INDEX_SKIPZERO)
		{
			if(iszero(*i, elat(*i, x)) == 0)
			{
				continue;     	
			}               	
		}

		// locate, where in the sequence this element would go
		uint16_t* lp = (*i)->locs;
		r = 0;
		while(*lp && (r = elcmp(*i, x, (*lp) - 1)) > 0)
			lp++;

		if(r == 0 && *lp && ((flags & INDEX_KIND) == INDEX_FIRST || (flags & INDEX_KIND) == INDEX_UNIQUE))
		{
			// already keyed
			if((flags & INDEX_KIND) == INDEX_UNIQUE)
			{
				return 1;
			}
		}
		else
		{
			if(!*lp)
			{
				// greatest entry
			}
			else if(r < 0 || (flags & INDEX_KIND) == INDEX_EXCLUDE || (flags & INDEX_KIND) == INDEX_MULTI)
			{
				// make room at this spot by shifting everything after it down by one
				// k is the 0-based element number we want to insert AT
				int k = ((char*)lp - (char*)(*i)->locs) / (sizeof((*i)->locs[0]));

				memmove(
						lp + 1
					, lp
					, (((*i)->len - k) * sizeof((*i)->locs[0]))
				);
			}

			// populate indexes off-by-one, so that for this loop, index 0 is invalid
			*lp = x + 1;

			(*i)->len++;
		}
	}

	// tick indexes down to their actual value
	for(x = 0; x < (*i)->len; x++)
		((*i)->locs[x])--;

	// for index: exclude, now remove duplicates
	if((flags & INDEX_KIND) == INDEX_EXCLUDE)
	{
		for(x = (*i)->len - 1; x > 0; x--)
		{
			for(y = x - 1; elcmp(*i, (*i)->locs[x], (*i)->locs[y]) == 0 && y >= 0; y--);

			if((++y) < x)
			{
				memmove(
						&(*i)->locs[y]
					, &(*i)->locs[x + 1]
					, ((*i)->len - x - 1) * sizeof((*i)->locs[0])
				);

				(*i)->len -= (x - y + 1);
				x = y;
			}
		}
	}

	// (*i)->len is <= nel that we used in the original malloc
	(*i) = realloc((*i), sizeof(*i[0]) + (sizeof((*i)->locs[0]) * (*i)->len));

	return 0;
}

void* idx_lookup_val(idx* i, void* key, int key_l)
{
	uint16_t* p = lookup(i, key, key_l);

	if(p)
	{
		if(i->flags & INDEX_OPTS & INDEX_DEREF)
			return *((void**)(i->coll + (sizeof(void*) * (*p))));
		else
			return i->coll + (i->ewid * (*p));
	}

	return 0;
}

int idx_lookup_idx(idx* i, void* key, int key_l)
{
	uint16_t* p = lookup(i, key, key_l);

	if(p)
		return *p;

	return -1;
}

int idx_enumerate(idx* i, void* _list, int* len)
{
	void*** list = (void***)_list;

	if(!xmalloc(list, i->len * (sizeof(**list))))
		return 1;

	int x;
	for(x = 0; x < i->len; x++)
	{
		if(i->flags & INDEX_OPTS & INDEX_DEREF)
			(*list)[x] = *((void**)(i->coll + (sizeof(void*) * i->locs[x])));
		else
			(*list)[x] = i->coll + (i->ewid * i->locs[x]);
	}

	*len = i->len;

	return 0;
}

void idx_sortby(idx* i)
{
	void doswap(uint16_t _a, uint16_t b)
	{
		uint16_t a = i->locs[_a];

		if(a == 0xFFFF)
			return;

		// mark off this position as having beeen completed
		i->locs[_a] = 0xFFFF;

		if(a != b)
		{
			// determine if the spot we are swapping with is also indexed
			int x;
			for(x = 0; x < i->len; x++)
			{
				if(i->locs[x] == b)
					break;
			}

			swap(i, a, b);

			// if it is, update it with the new location
			if(x < i->len)
				i->locs[x] = a;
		}
	};

	// place each item in the collection in its position as indicated by the index
	int x;
	for(x = 0; x < i->len; x++)
		doswap(x, x);

	// the collection is now sorted, rewrite the index
	for(x = 0; x < i->len; x++)
		i->locs[x] = x;
}

void idx_free(idx* i)
{
	free(i);
}

void idx_xfree(idx** i)
{
	idx_free(*i);
	*i = 0;
}
