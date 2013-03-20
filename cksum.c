#include "cksum.h"

//
// public
//

uint32_t cksum(const void * v, size_t l)
{
	const char * s = v;
	uint32_t h = 0;

	int x;
	for(x = 0; x < l; x++)
	{
		h += s[x];
		h += (h << 10);
		h ^= (h >> 6);
	}

	h += (h << 3);
	h ^= (h >> 11);
	h += (h << 15);

	return h;
}
