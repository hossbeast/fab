#ifndef config_STATES_H
#define config_STATES_H
extern int config_numstates;
extern int * config_statenumbers;
extern char ** config_statenames;
enum config_statenames {
config_INITIAL = 0,
config_initial = 1,
config_multilinecomment = 2,
config_quoted = 3,
};
#endif
