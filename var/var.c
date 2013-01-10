#include <string.h>

#include <listwise/lstack.h>

#include "var.h"

#include "control.h"
#include "log.h"

//
// static
//

static int var_set(map * vmap, char * s, lstack * ls, uint64_t lo)
{
	log_start(lo, "%10s = [ ", s);

	int i = 0;
	int j = 0;

	LSTACK_ITERATE(ls, i, go);
	if(go)
	{
		if(j++)
			log_add(" ");

		if(ls->s[0].s[i].type)
		{
			char * vs = 0;
			int    vl = 0;
			lstack_string(ls, 0, i, &vs, &vl);

			log_add("[%hhu]%p (%.*s)"
				, ls->s[0].s[i].type
				, *(void**)ls->s[0].s[i].s
				, vl
				, vs
			);
		}
		else
			log_add("%.*s", ls->s[0].s[i].l, ls->s[0].s[i].s);
	}
	LSTACK_ITEREND;
	log_finish(" ]");

	// save the resultant list
	fatal(map_set, vmap, s, strlen(s), &ls, sizeof(ls));

	finally : coda;
}

//
// public
//

int var_set_auto(map * vmap, char * s, lstack * ls)
{
	return var_set(vmap, s, ls, L_VARAUTO);
}

int var_set_user(map * vmap, char * s, lstack * ls)
{
	return var_set(vmap, s, ls, L_VARUSER);
}
