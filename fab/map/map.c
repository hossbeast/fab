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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"

#include "control.h"
#include "xmem.h"
#include "xstring.h"
#include "macros.h"

#define SATURATION 0.45f			/* for 1000-sized table, reallocate at 450 keys */

#define restrict __restrict
typedef struct
{
	int			d;			// deleted

	size_t	a;			// allocated
	size_t	l;			// length

	char		p[];		// payload
} __attribute__((packed)) slot;

struct map_t
{
	size_t			l;				// table length, in elements (always a power of two)
	uint32_t		lm;				// bitmask equal to table length - 1
	slot**			tk;				// key table
	slot**			tv;				// val table

	size_t			kc;				// number of stored k/v pairs
	void (*destructor)(void*, void*);
};

static uint32_t hashkey(const map* const restrict m, const char* const restrict k, const int kl)
{
/*
 * this hashing algorithm taken from the jenkins hash function
 *   http://en.wikipedia.org/wiki/Jenkins_hash_function
 */

	uint32_t jenkins(const map* const restrict m, const char* const k, const int kl)
	{
		int x;
		uint32_t h = 0;
		for(x = 0; x < kl; x++)
		{
			h += k[x];
			h += (h << 10);
			h ^= (h >> 6);
		}
		h += (h << 3);
		h ^= (h >> 11);
		h += (h << 15);

		return h;
	};

	return jenkins(m, k, kl) & m->lm;
}

/// lookup
//
// lookup the given key, returns the index found in *i
//
// returns
//  0  : found
//  1  : not found, and a deleted slot is in the probe sequence (i)
//  -1 : not found
//
static int lookup(const map* const restrict m, const char* const restrict k, const int kl, uint32_t* const restrict i)
{
	*i = hashkey(m, k, kl);

	uint32_t ij = 0;
	uint32_t * ijp = &ij;

	while(1)
	{
		if(m->tk[*i] && (m->tk[*i]->l || m->tk[*i]->d))
		{
			if(m->tk[*i]->d)
			{
				if(ijp)
					*ijp = *i;

				ijp = 0;
			}
			else if(m->tk[*i]->l == kl)
			{
				if(memcmp(k, m->tk[*i]->p, kl) == 0)
					return 0;
			}
		}
		else if(ijp == 0)
		{
			*i = ij;
			return 1;
		}
		else
		{
			return -1;
		}

		// ring increment
		(*i)++;
		(*i) &= m->lm;
	}
}

//// [[ API ]] /////

int map_create(map** const restrict m, void (*destructor)(void*, void*))
{
	fatal(xmalloc, m, sizeof(*m[0]));

	// compute initial table size for 100 keys @ given saturation
	(*m)->l = 100 * (1 / SATURATION);

	// round up to the next highest power of 2
	(*m)->l--;
	(*m)->l |= (*m)->l >> 1;
	(*m)->l |= (*m)->l >> 2;
	(*m)->l |= (*m)->l >> 4;
	(*m)->l |= (*m)->l >> 8;
	(*m)->l |= (*m)->l >> 16;
	(*m)->l++;

	(*m)->lm = (*m)->l - 1;

	fatal(xmalloc, &(*m)->tk, sizeof(*(*m)->tk) * (*m)->l);
	fatal(xmalloc, &(*m)->tv, sizeof(*(*m)->tv) * (*m)->l);

	(*m)->destructor = destructor;

	finally : coda;
}

int map_set(map* const restrict m, const void* const restrict k, int kl, const void* const restrict v, int vl, void * restrict rv)
{
	slot** ks = 0;
	slot** vs = 0;
	slot** uk = 0;
	slot** uv = 0;

	// perform lookup, see if this key is already mapped to something
	uint32_t i = 0;
	int r = lookup(m, k, kl, &i);
	if(r == 0)
	{
		// this entry will be updated with the new value
		if(m->destructor)
			m->destructor(0, m->tv[i]->p);

		m->tv[i]->l = 0;
	}
	else if(r == 1)
	{
		// not found, and a suitable deleted slot exists
	}
	else if(m->kc == (int)(m->l * SATURATION))
	{
		// not found, and saturation has been reached

		// linear list of set keys and values
		fatal(xmalloc, &ks, m->kc * sizeof(*ks));
		fatal(xmalloc, &vs, m->kc * sizeof(*vs));

		// partial list of unset (but allocated) keys and values
		fatal(xmalloc, &uk, (m->l - m->kc) * sizeof(*uk));
		fatal(xmalloc, &uv, (m->l - m->kc) * sizeof(*uv));

		// get list of set keys/values
		int y = 0;
		int x = 0;
		for(x = 0; x < m->l; x++)
		{
			if(m->tk[x])
				m->tk[x]->d = 0;

			if(m->tk[x] && m->tk[x]->l)
			{
				ks[y] = m->tk[x];
				m->tk[x] = 0;

				vs[y] = m->tv[x];
				m->tv[x] = 0;
				
				y++;
			}
		}

		// reallocate tables
		fatal(xrealloc
			, &m->tk
			, 1
			, sizeof(*m->tk) * (m->l << 2)
			, sizeof(*m->tk) * (m->l)
		);
		fatal(xrealloc
			, &m->tv
			, 1
			, sizeof(*m->tv) * (m->l << 2)
			, sizeof(*m->tv) * (m->l)
		);

		m->l <<= 2;
		m->lm = m->l - 1;

		// reassociate all k/v pairs
		y = 0;
		for(x = 0; x < m->kc; x++)
		{
			uint32_t j = hashkey(m, ks[x]->p, ks[x]->l);

			while(1)
			{
				if(!m->tk[j] || !m->tk[j]->l)
				{
					if(m->tk[j])
					{
						// displacement
						uk[y] = m->tk[j];
						uv[y] = m->tv[j];
						y++;
					}

					m->tk[j] = ks[x];
					m->tv[j] = vs[x];
					break;
				}

				j++;
				j &= m->lm;
			}
		}

		// add unset (but allocated) k/v which were displaced
		uint32_t j = 0;
		for(x = 0; x < y; x++)
		{
			while(m->tk[j])
				j++;

			m->tk[j] = uk[x];
			m->tv[j] = uv[x];
		}

		// re-hash the current key
		lookup(m, k, kl, &i);
	}

	// allocate the value slot if necessary
	if(m->tv[i] == 0 || vl >= m->tv[i]->a)
	{
		fatal(xrealloc
			, &m->tv[i]
			, 1
			, sizeof(*m->tv[i]) + MAX(vl, sizeof(void*)) + 1
			, sizeof(*m->tv[i]) + (m->tv[i] ? m->tv[i]->a : 0)
		);

		m->tv[i]->a = MAX(vl, sizeof(void*)) + 1;
	}

	if(!v || vl < sizeof(void*))
	{
		memset(m->tv[i]->p, 0, sizeof(void*) + 1);
	}
	if(v)
	{
		memcpy(m->tv[i]->p, v, vl);
		m->tv[i]->p[vl] = 0;
	}
	m->tv[i]->l = vl;

	// copy the key
	if(r)
	{
		if(m->tk[i] == 0 || kl >= m->tk[i]->a)
		{
			fatal(xrealloc
				, &m->tk[i]
				, 1
				, sizeof(*m->tk[i]) + kl + 1
				, sizeof(*m->tk[i]) + (m->tk[i] ? m->tk[i]->a : 0)
			);

			m->tk[i]->a = kl + 1;
		}

		memcpy(m->tk[i]->p, k, kl);
		m->tk[i]->p[kl] = 0;
		m->tk[i]->l = kl;
		m->tk[i]->d = 0;

		m->kc++;
	}

finally:
	free(ks);
	free(vs);
	free(uk);
	free(uv);

	if(_coda_r == 0 && rv)
		*(void**)rv = m->tv[i]->p;
coda;
}

void* map_get(const map* const restrict m, const void* const restrict k, int kl)
{
	// perform lookup
	uint32_t i = 0;
	if(lookup(m, k, kl, &i) == 0)
		return m->tv[i]->p;

	return 0;
}

int map_count(const map* const restrict m)
{
	return m->kc;
}

void map_clear(map* const restrict m)
{
	// reset all lengths; all allocations remain intact
	int x;
	for(x = 0; x < m->l; x++)
	{
		if(m->tk[x] && m->tk[x]->l)
		{
			if(m->destructor)
			{
				m->destructor(
						m->tk[x]->p
					, m->tv[x]->p
				);
			}
			m->tk[x]->l = 0;
			m->tk[x]->d = 0;
			m->tv[x]->l = 0;
		}
	}

	m->kc = 0;
}

int map_delete(map* const restrict m, const void* const restrict k, int kl)
{
	uint32_t i = 0;
	if(lookup(m, k, kl, &i) == 0)
	{
		if(m->destructor)
		{
			m->destructor(
				  m->tk[i]->p
				, m->tv[i]->p
			);
		}

		m->tk[i]->l = 0;
		m->tk[i]->d = 1;
		m->tv[i]->l = 0;

		m->kc--;

		return 0;
	}

	return 1;
}

int map_keys(const map* const restrict m, void* const restrict t, int* const restrict c)
{
	fatal(xmalloc, t, m->kc * sizeof(void*));
	*c = 0;

	int x;
	for(x = 0; x < m->l; x++)
	{
		if(m->tk[x] && m->tk[x]->l)
		{
			(*(void***)t)[(*c)++] = m->tk[x]->p;
		}
	}

	finally : coda;
}

int map_values(const map* const restrict m, void* const restrict t, int* const restrict c)
{
	fatal(xmalloc, t, m->kc * sizeof(void*));
	*c = 0;

	int x;
	for(x = 0; x < m->l; x++)
	{
		if(m->tv[x] && m->tv[x]->l)
		{
			(*(void***)t)[(*c)++] = m->tv[x]->p;
		}
	}

	finally : coda;
}

int map_clone(map* const restrict dst, const map * const restrict src)
{
	int x;
	map_clear(dst);

	// the dst map must have precisely the same size in order for the probe sequences to work 
	if(dst->l != src->l)
	{
		if(dst->l > src->l)
		{
			for(x = src->l; x < dst->l; x++)
			{
				free(dst->tk[x]);
				free(dst->tv[x]);
			}
		}

		fatal(xrealloc, &dst->tk, 1, sizeof(*dst->tk) * src->l, 0);
		fatal(xrealloc, &dst->tv, 1, sizeof(*dst->tv) * src->l, 0);

		dst->l = src->l;
		dst->lm = src->lm;
	}

	for(x = 0; x < src->l; x++)
	{
		if(src->tk[x])
		{
			if(src->tk[x]->d)
			{
				if(dst->tk[x] == 0)
				{
					fatal(xmalloc, &dst->tk[x], sizeof(*dst->tk[x]));
				}

				dst->tk[x]->d = 1;
				dst->tk[x]->l = 0;
			}
			else if(src->tk[x]->l)
			{
				// reallocate key slot if necessary
				if(dst->tk[x] == 0 || src->tk[x]->l >= dst->tk[x]->a)
				{
					fatal(xrealloc
						, &dst->tk[x]
						, 1
						, sizeof(*dst->tk[x]) + src->tk[x]->l + 1
						, sizeof(*dst->tk[x]) + (dst->tk[x] ? dst->tk[x]->a : 0)
					);

					dst->tk[x]->a = src->tk[x]->l + 1;
				}

				// copy the key
				memcpy(dst->tk[x]->p, src->tk[x]->p, src->tk[x]->l);
				dst->tk[x]->p[src->tk[x]->l] = 0;
				dst->tk[x]->l = src->tk[x]->l;
				dst->tk[x]->d = 0;

				// reallocate the value slot if necessary
				if(dst->tv[x] == 0 || src->tv[x]->l >= dst->tv[x]->a)
				{
					fatal(xrealloc
						, &dst->tv[x]
						, 1
						, sizeof(*dst->tv[x]) + MAX(src->tv[x]->l, sizeof(void*)) + 1
						, sizeof(*dst->tv[x]) + (dst->tv[x] ? dst->tv[x]->a : 0)
					);

					dst->tv[x]->a = MAX(src->tv[x]->l, sizeof(void*)) + 1;
				}

				// copy the value
				if(src->tv[x]->l < sizeof(void*))
				{
					memset(dst->tv[x]->p, 0, sizeof(void*) + 1);
				}

				memcpy(dst->tv[x]->p, src->tv[x]->p, src->tv[x]->l);
				dst->tv[x]->p[src->tv[x]->l] = 0;

				dst->tv[x]->l = src->tv[x]->l;
				dst->tv[x]->d = 0;
			}
		}
	}

	dst->kc = src->kc;

	finally : coda;
}

void map_free(map* const restrict m)
{
	if(m)		// free-like semantics
	{
		int x;
		for(x = 0; x < m->l; x++)
		{
			if(m->tk[x] && m->tk[x]->l)
			{
				if(m->destructor)
					m->destructor(m->tk[x]->p, m->tv[x]->p);
			}

			free(m->tk[x]);
			free(m->tv[x]);
		}
		
		free(m->tk);
		free(m->tv);
	}

	free(m);
}

void map_xfree(map** const restrict m)
{
	map_free(*m);
	*m = 0;
}
