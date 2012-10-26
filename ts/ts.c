#include "ts.h"

#include "xmem.h"
#include "pstring.h"

#include "control.h"

int ts_mk(ts ** ts)
{
	fatal(xmalloc, ts, sizeof(**ts));
}

void ts_free(ts * ts)
{
	if(ts)
	{
		pstring_free(ts->cmd_path);
		pstring_free(ts->cmd_txt);
		pstring_free(ts->stdo_path);
		pstring_free(ts->stdo_txt);
		pstring_free(ts->stde_path);
		pstring_free(ts->stde_txt);
	}

	free(ts);
}
