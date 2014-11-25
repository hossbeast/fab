#ifndef transform_STATES_H
#define transform_STATES_H
extern int transform_numstates;
extern int * transform_statenumbers;
extern char ** transform_statenames;
enum transform_statenames {
transform_INITIAL = 0,
transform_ws = 1,
transform_multilinecomment = 2,
transform_emptyarg = 3,
transform_slash_norefs = 4,
transform_slash_dorefs = 5,
transform_comma_norefs = 6,
transform_comma_dorefs = 7,
transform_dot_norefs = 8,
transform_dot_dorefs = 9,
transform_colon_norefs = 10,
transform_colon_dorefs = 11,
transform_semicolon_norefs = 12,
transform_semicolon_dorefs = 13,
transform_braces = 14,
transform_braces_norefs = 15,
transform_braces_dorefs = 16,
transform_brackets = 17,
transform_brackets_norefs = 18,
transform_brackets_dorefs = 19,
transform_parens = 20,
transform_parens_norefs = 21,
transform_parens_dorefs = 22,
transform_angles = 23,
transform_angles_norefs = 24,
transform_angles_dorefs = 25,
};
#endif
