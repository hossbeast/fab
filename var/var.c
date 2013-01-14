#include <string.h>

#include <listwise/lstack.h>

#include "var.h"

#include "control.h"
#include "log.h"

//
// public
//

int var_set(map * vmap, char * s, lstack * ls, int sticky)
{
	if(sticky || map_get(vmap, s, strlen(s)) == 0)
	{
		uint64_t lo = 0;

		if(strcmp(s, "@") == 0)
			lo = L_VARAUTO;
		else if(strcmp(s, "#") == 0)
			lo = L_VARAUTO;
		else if(strcmp(s, "<") == 0)
			lo = L_VARAUTO;
		else
			lo = L_VARUSER;

		if(log_would(lo))
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
		}

		fatal(map_set, vmap, s, strlen(s), &ls, sizeof(ls));
	}

	finally : coda;
}
