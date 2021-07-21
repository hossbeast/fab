#ifndef _PROBES_H
#define _PROBES_H

struct yyu_parser;
struct yyu_location;

extern unsigned short yyu_token_semaphore;
extern unsigned short yyu_pushstate_semaphore;
extern unsigned short yyu_popstate_semaphore;

void popstate_probe(const struct yyu_parser * restrict parser, int prev, int next);
void pushstate_probe(const struct yyu_parser * restrict parser, int prev, int next);
void token_probe(
    struct yyu_parser * const restrict xtra
  , int token
  , void * const restrict lval
  , struct yyu_location * const restrict lloc
  , char * restrict text
  , size_t bytes
);

#endif
