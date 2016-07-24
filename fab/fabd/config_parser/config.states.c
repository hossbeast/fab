char ** config_statenames = (char *[]) {
[0] = "INITIAL",
[1] = "initial",
[2] = "multilinecomment",
[3] = "quoted",
};
int * config_statenumbers = (int []) {
0,
1,
2,
3,
};
int config_numstates = sizeof((int []) {
0,
1,
2,
3,
}) / sizeof(int);
