#include <string.h>

#include <listwise/lstack.h>

#include "var.h"

#include "control.h"
#include "log.h"

int var_set(map * vmap, char * s, lstack * ls)
{
	log_start(L_VAR, "%10s = [ ", s);

	int i = 0;
	int j = 0;

	LSTACK_LOOP_ITER(ls, i, go);
	if(go)
	{
		if(j++)
			log_add(" ");

		if(ls->s[0].s[i].type)
		{
			char * vs = 0;
			int    vl = 0;
			lstack_getstring(ls, 0, i, &vs, &vl);

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
	LSTACK_LOOP_DONE;
	log_finish(" ]");

	// save the resultant list
	fatal(map_set, vmap, s, strlen(s), &ls, sizeof(ls));
}
