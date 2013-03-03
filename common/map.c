#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"

#include "control.h"
#include "xmem.h"
#include "xstring.h"

#define SATURATION 0.45f			/* for 1000-sized table, reallocate at 450 keys */

#define restrict __restrict
typedef struct
{
	size_t	a;			// allocated, in elements
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
// returns 1 if found, 0 otherwise
//
static int lookup(const map* const restrict m, const char* const restrict k, const int kl, uint32_t* const restrict i)
{
	*i = hashkey(m, k, kl);

	while(1)
	{
		if(m->tk[*i] && m->tk[*i]->l)
		{
			if(m->tk[*i]->l == kl)
			{
				if(memcmp(k, m->tk[*i]->p, kl) == 0)
					return 1;
			}
		}
		else
		{
			return 0;
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

void* map_set(map* const restrict m, const void* const restrict k, int kl, const void* const restrict v, int vl)
{
	slot** ks = 0;
	slot** vs = 0;
	slot** uk = 0;
	slot** uv = 0;

	// perform lookup, see if this key is already mapped to something
	uint32_t i = 0;
	int isupdate = lookup(m, k, kl, &i);
	if(isupdate)
	{
		// this entry will be updated with the new value
		if(m->destructor)
			m->destructor(0, m->tv[i]->p);

		m->tv[i]->l = 0;
	}
	else if(m->kc == (int)(m->l * SATURATION))
	{
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
			, sizeof(*m->tv[i]) + vl + 1
			, sizeof(*m->tv[i]) + (m->tv[i] ? m->tv[i]->a : 0)
		);

		m->tv[i]->a = vl + 1;
	}

	if(v)
		memcpy(m->tv[i]->p, v, vl);
	m->tv[i]->p[vl] = 0;
	m->tv[i]->l = vl;

	// copy the key
	if(!isupdate)
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

		m->kc++;
	}

finally:
	free(ks);
	free(vs);
	free(uk);
	free(uv);

	if(_coda_r)
		return m->tv[i]->p;

	return 0;
}

void* map_get(const map* const restrict m, const void* const restrict k, int kl)
{
	// perform lookup
	uint32_t i = 0;
	if(lookup(m, k, kl, &i))
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
			m->tv[x]->l = 0;
		}
	}

	m->kc = 0;
}

int map_delete(map* const restrict m, const void* const restrict k, int kl)
{
	uint32_t i = 0;
	if(lookup(m, k, kl, &i))
	{
		if(m->destructor)
		{
			m->destructor(
				  m->tk[i]->p
				, m->tv[i]->p
			);
		}

		m->tk[i]->l = 0;
		m->tv[i]->l = 0;

		m->kc--;

		return 1;
	}

	return 0;
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
