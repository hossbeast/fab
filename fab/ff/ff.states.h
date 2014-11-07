#ifndef ff_STATES_H
#define ff_STATES_H
extern int ff_numstates;
extern int * ff_statenumbers;
extern char ** ff_statenames;
enum ff_statenames {
ff_INITIAL = 0,
ff_quoteword = 1,
ff_transform = 2,
ff_command = 3,
ff_commandchecklit = 4,
ff_commandlit = 5,
ff_blockcheck = 6,
ff_list = 7,
ff_varref = 8,
ff_nofile = 9,
ff_multilinecomment = 10,
};
#endif
