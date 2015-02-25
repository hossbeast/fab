#ifndef cfg_STATES_H
#define cfg_STATES_H
extern int cfg_numstates;
extern int * cfg_statenumbers;
extern char ** cfg_statenames;
enum cfg_statenames {
cfg_INITIAL = 0,
cfg_quoted = 1,
cfg_transform = 2,
cfg_command = 3,
cfg_commandchecklit = 4,
cfg_commandlit = 5,
cfg_blockcheck = 6,
cfg_list = 7,
cfg_varref = 8,
cfg_nofile = 9,
cfg_multilinecomment = 10,
};
#endif
