#ifndef generator_STATES_H
#define generator_STATES_H
extern int generator_numstates;
extern int * generator_statenumbers;
extern char ** generator_statenames;
enum generator_statenames {
generator_INITIAL = 0,
generator_ws = 1,
generator_multilinecomment = 2,
generator_emptyarg = 3,
generator_slash_norefs = 4,
generator_slash_dorefs = 5,
generator_comma_norefs = 6,
generator_comma_dorefs = 7,
generator_dot_norefs = 8,
generator_dot_dorefs = 9,
generator_colon_norefs = 10,
generator_colon_dorefs = 11,
generator_semicolon_norefs = 12,
generator_semicolon_dorefs = 13,
generator_braces = 14,
generator_braces_norefs = 15,
generator_braces_dorefs = 16,
generator_brackets = 17,
generator_brackets_norefs = 18,
generator_brackets_dorefs = 19,
generator_parens = 20,
generator_parens_norefs = 21,
generator_parens_dorefs = 22,
generator_angles = 23,
generator_angles_norefs = 24,
generator_angles_dorefs = 25,
};
#endif
