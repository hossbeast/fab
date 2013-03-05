#ifndef _LISTWISE_OPS_H
#define _LISTWISE_OPS_H

// available operators - dloaded at init time
extern operator** 		g_ops;
extern int						g_ops_l;

// lookup an operator by name
operator*							op_lookup(char* s, int l);

#endif
