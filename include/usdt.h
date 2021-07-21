#ifndef _FAB_USDT_H
#define _FAB_USDT_H

#include <sys/sdt.h>

#define PROBE(provider, probe) STAP_PROBE(provider, probe)
#define PROBE1(provider, probe, a) STAP_PROBE1(provider, probe, a)
#define PROBE2(provider, probe, a, b) STAP_PROBE2(provider, probe, a, b)

#endif
