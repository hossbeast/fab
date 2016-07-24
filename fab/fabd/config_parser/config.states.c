char ** config_statenames = (char *[]) {
[0] = "INITIAL",
[1] = "multilinecomment",
[2] = "quoted",
};
int * config_statenumbers = (int []) {
0,
1,
2,
};
int config_numstates = sizeof((int []) {
0,
1,
2,
}) / sizeof(int);
