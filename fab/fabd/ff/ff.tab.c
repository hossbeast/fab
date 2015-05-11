
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 1

/* "%code top" blocks.  */

/* Line 171 of yacc.c  */
#line 18 "fab/fabd/ff/ff.y"

	#include <stdio.h>
	#include <stdint.h>
	#include <string.h>
	#include <stdlib.h>

	#include "xlinux.h"

	#include "ff.parse.h"
	#include "ffn.h"

	// defined in ff.lex.o
	int ff_yylex(void* yylvalp, void* yylloc, void* scanner);



/* Line 171 of yacc.c  */
#line 88 "fab/fabd/ff/ff.tab.c"
/* Substitute the variable and function names.  */
#define yyparse         ff_yyparse
#define yylex           ff_yylex
#define yyerror         ff_yyerror
#define yylval          ff_yylval
#define yychar          ff_yychar
#define yydebug         ff_yydebug
#define yynerrs         ff_yynerrs
#define yylloc          ff_yylloc

/* Copy the first part of user declarations.  */


/* Line 189 of yacc.c  */
#line 103 "fab/fabd/ff/ff.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     WORD = 258,
     WS = 259,
     LF = 260,
     ONCE = 261
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 44 "fab/fabd/ff/ff.y"

	struct ff_node*			node;



/* Line 214 of yacc.c  */
#line 151 "fab/fabd/ff/ff.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 176 "fab/fabd/ff/ff.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  32
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   127

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  26
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  27
/* YYNRULES -- Number of rules.  */
#define YYNRULES  68
/* YYNRULES -- Number of states.  */
#define YYNSTATES  112

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   261

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     6,    12,     2,     2,
      23,    24,    10,    22,    15,    20,     8,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     9,    21,
       2,    19,     2,     2,     7,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    13,     2,    14,    11,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    17,     2,    18,    16,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,    25
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    11,    15,    20,    25,
      30,    36,    42,    46,    49,    55,    60,    66,    71,    74,
      76,    78,    80,    82,    84,    87,    91,    93,    96,    98,
     101,   105,   107,   109,   111,   113,   115,   117,   119,   121,
     123,   128,   132,   135,   139,   144,   150,   156,   163,   167,
     169,   171,   173,   177,   181,   183,   187,   190,   195,   200,
     206,   213,   219,   221,   224,   226,   228,   230,   233
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      27,     0,    -1,    28,    -1,    -1,    28,    39,    -1,    39,
      -1,    30,     9,    30,    -1,    30,     9,    10,    30,    -1,
      30,     9,    11,    30,    -1,    30,     9,     9,    30,    -1,
      30,     9,     9,    10,    30,    -1,    30,     9,     9,    11,
      30,    -1,    13,    31,    14,    -1,    13,    14,    -1,    13,
      31,    16,    34,    14,    -1,    13,    16,    34,    14,    -1,
      13,    31,    16,    30,    14,    -1,    13,    16,    30,    14,
      -1,    31,    32,    -1,    32,    -1,    33,    -1,    38,    -1,
      36,    -1,    30,    -1,     6,    38,    -1,     6,    11,    38,
      -1,    35,    -1,    35,    38,    -1,    38,    -1,     7,    37,
      -1,    37,     8,    38,    -1,    38,    -1,     3,    -1,     5,
      -1,    41,    -1,    42,    -1,    29,    -1,    47,    -1,    48,
      -1,    40,    -1,    25,    17,    28,    18,    -1,    52,    19,
      30,    -1,    22,    30,    -1,    22,    30,    36,    -1,    22,
      30,    23,    24,    -1,    22,    30,    23,    24,    36,    -1,
      22,    30,    23,    43,    24,    -1,    22,    30,    23,    43,
      24,    36,    -1,    43,    21,    44,    -1,    44,    -1,    45,
      -1,    46,    -1,    33,    19,    30,    -1,    33,    19,    33,
      -1,    52,    -1,    33,    20,    33,    -1,    20,    52,    -1,
      29,    17,    49,    18,    -1,    30,    17,    49,    18,    -1,
      30,     9,    17,    49,    18,    -1,    30,     9,     9,    17,
      49,    18,    -1,    30,    12,    17,    49,    18,    -1,    50,
      -1,    50,    51,    -1,    51,    -1,    30,    -1,    38,    -1,
      52,    33,    -1,    33,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   108,   108,   113,   119,   123,   126,   130,   134,   138,
     142,   146,   152,   156,   160,   164,   168,   172,   179,   183,
     187,   188,   189,   190,   194,   198,   205,   212,   216,   220,
     227,   231,   235,   239,   245,   246,   247,   248,   249,   250,
     254,   261,   268,   272,   276,   280,   284,   288,   295,   299,
     303,   304,   308,   313,   318,   326,   331,   339,   344,   348,
     353,   360,   367,   374,   378,   382,   383,   387,   391
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "\"WORD\"", "\"WS\"", "\"LF\"", "'$'",
  "'@'", "'.'", "':'", "'*'", "'^'", "'%'", "'['", "']'", "','", "'~'",
  "'{'", "'}'", "'='", "'-'", "';'", "'+'", "'('", "')'", "\"ONCE\"",
  "$accept", "ff", "statements", "dependency", "list", "listparts",
  "listpart", "varref", "transform", "transformparts", "nofile",
  "nofileparts", "word", "statement", "onceblock", "varassign",
  "invocation", "varsettings", "varsetting", "varlock", "varlink",
  "fabrication", "discovery", "command", "commandparts", "commandpart",
  "varrefs", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,    36,    64,    46,    58,
      42,    94,    37,    91,    93,    44,   126,   123,   125,    61,
      45,    59,    43,    40,    41,   261
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    26,    27,    27,    28,    28,    29,    29,    29,    29,
      29,    29,    30,    30,    30,    30,    30,    30,    31,    31,
      32,    32,    32,    32,    33,    33,    34,    35,    35,    36,
      37,    37,    38,    38,    39,    39,    39,    39,    39,    39,
      40,    41,    42,    42,    42,    42,    42,    42,    43,    43,
      44,    44,    45,    45,    45,    46,    46,    47,    47,    47,
      47,    48,    49,    50,    50,    51,    51,    52,    52
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     3,     4,     4,     4,
       5,     5,     3,     2,     5,     4,     5,     4,     2,     1,
       1,     1,     1,     1,     2,     3,     1,     2,     1,     2,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       4,     3,     2,     3,     4,     5,     5,     6,     3,     1,
       1,     1,     3,     3,     1,     3,     2,     4,     4,     5,
       6,     5,     1,     2,     1,     1,     1,     2,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     0,     0,     0,     0,     2,    36,     0,    68,
       5,    39,    34,    35,    37,    38,     0,    32,    33,     0,
      24,     0,    13,     0,    23,     0,    19,    20,    22,    21,
      42,     0,     1,     4,     0,     0,     0,     0,     0,    67,
      25,    29,    31,     0,     0,    26,    28,    12,     0,    18,
       0,    43,     0,    65,    66,     0,    62,    64,     0,     0,
       0,     0,     6,     0,     0,    41,     0,    17,    15,    27,
       0,     0,     0,    44,    68,     0,    49,    50,    51,    54,
      40,    57,    63,     0,     0,     0,     9,     7,     8,     0,
       0,    58,    30,    16,    14,    56,    45,     0,     0,     0,
      46,    10,    11,     0,    59,    61,    52,    53,    55,    48,
      47,    60
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     5,     6,     7,    53,    25,    26,     9,    44,    45,
      28,    41,    54,    10,    11,    12,    13,    75,    76,    77,
      78,    14,    15,    55,    56,    57,    16
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -44
static const yytype_int8 yypact[] =
{
      24,    52,    74,    -4,    -6,    14,    24,    34,    57,   -44,
     -44,   -44,   -44,   -44,   -44,   -44,    28,   -44,   -44,    67,
     -44,    67,   -44,    86,   -44,   100,   -44,   -44,   -44,   -44,
      10,    24,   -44,   -44,    86,   108,    45,    86,    -4,   -44,
     -44,    59,   -44,    64,    71,    67,   -44,   -44,    86,   -44,
      12,   -44,     2,   -44,   -44,    55,    86,   -44,    98,    -4,
      -4,    86,   -44,    86,    75,   -44,    67,   -44,   -44,   -44,
      78,    80,    89,    97,    56,   -11,   -44,   -44,   -44,    89,
     -44,   -44,   -44,    -4,    -4,    86,   -44,   -44,   -44,    92,
      94,   -44,   -44,   -44,   -44,    89,   -44,    48,    89,     6,
      97,   -44,   -44,   102,   -44,   -44,   -44,   -44,   -44,   -44,
     -44,   -44
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -44,   -44,    65,   -44,     0,   -44,    99,     3,    79,   -44,
     -29,   -44,    20,    -2,   -44,   -44,   -44,   -44,    23,   -44,
     -44,   -44,   -44,   -21,   -44,    70,   -43
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
       8,    51,    24,    30,    33,    27,     8,    79,     1,     2,
      99,    31,     1,   100,    32,     2,    64,    21,     1,    39,
      80,    20,    29,    43,     3,    24,    72,     4,    27,    95,
       1,     8,    72,    50,     1,    62,    73,     2,    65,    40,
      89,    42,    90,    46,    96,    29,     3,    38,    70,     4,
      33,    34,     8,    74,     1,    17,    79,    18,    86,    87,
      88,     2,    63,    19,   103,    69,    35,    66,    46,    36,
      17,   110,    18,    81,    37,    97,    98,    17,    67,    18,
       1,    21,    39,   101,   102,    68,    92,     2,    22,    17,
      23,    18,    93,    91,    94,     1,    52,   106,    39,     2,
     107,   108,    74,    17,    21,    18,     1,    21,    83,    84,
     104,     2,   105,     2,    47,    85,    48,    58,    59,    60,
     111,     2,   109,     0,    49,    61,    82,    71
};

static const yytype_int8 yycheck[] =
{
       0,    30,     2,     3,     6,     2,     6,    50,     6,    13,
      21,    17,     6,    24,     0,    13,    37,     7,     6,    16,
      18,     1,     2,    23,    22,    25,    20,    25,    25,    72,
       6,    31,    20,    23,     6,    35,    24,    13,    38,    19,
      61,    21,    63,    23,    73,    25,    22,    19,    48,    25,
      52,    17,    52,    50,     6,     3,    99,     5,    58,    59,
      60,    13,    17,    11,    85,    45,     9,     8,    48,    12,
       3,   100,     5,    18,    17,    19,    20,     3,    14,     5,
       6,     7,    79,    83,    84,    14,    66,    13,    14,     3,
      16,     5,    14,    18,    14,     6,    31,    97,    95,    13,
      97,    98,    99,     3,     7,     5,     6,     7,    10,    11,
      18,    13,    18,    13,    14,    17,    16,     9,    10,    11,
      18,    13,    99,    -1,    25,    17,    56,    48
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     6,    13,    22,    25,    27,    28,    29,    30,    33,
      39,    40,    41,    42,    47,    48,    52,     3,     5,    11,
      38,     7,    14,    16,    30,    31,    32,    33,    36,    38,
      30,    17,     0,    39,    17,     9,    12,    17,    19,    33,
      38,    37,    38,    30,    34,    35,    38,    14,    16,    32,
      23,    36,    28,    30,    38,    49,    50,    51,     9,    10,
      11,    17,    30,    17,    49,    30,     8,    14,    14,    38,
      30,    34,    20,    24,    33,    43,    44,    45,    46,    52,
      18,    18,    51,    10,    11,    17,    30,    30,    30,    49,
      49,    18,    38,    14,    14,    52,    36,    19,    20,    21,
      24,    30,    30,    49,    18,    18,    30,    33,    33,    44,
      36,    18
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (&yylloc, scanner, parm, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, &yylloc, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, &yylloc, scanner)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location, scanner, parm); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void* scanner, parse_param* parm)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, scanner, parm)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    void* scanner;
    parse_param* parm;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
  YYUSE (scanner);
  YYUSE (parm);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void* scanner, parse_param* parm)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp, scanner, parm)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    void* scanner;
    parse_param* parm;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, scanner, parm);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, void* scanner, parse_param* parm)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule, scanner, parm)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
    void* scanner;
    parse_param* parm;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       , scanner, parm);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule, scanner, parm); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, void* scanner, parse_param* parm)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp, scanner, parm)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
    void* scanner;
    parse_param* parm;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (scanner);
  YYUSE (parm);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {
      case 28: /* "statements" */

/* Line 1000 of yacc.c  */
#line 104 "fab/fabd/ff/ff.y"
	{ ffn_free((yyvaluep->node)); };

/* Line 1000 of yacc.c  */
#line 1197 "fab/fabd/ff/ff.tab.c"
	break;
      case 29: /* "dependency" */

/* Line 1000 of yacc.c  */
#line 104 "fab/fabd/ff/ff.y"
	{ ffn_free((yyvaluep->node)); };

/* Line 1000 of yacc.c  */
#line 1206 "fab/fabd/ff/ff.tab.c"
	break;
      case 30: /* "list" */

/* Line 1000 of yacc.c  */
#line 104 "fab/fabd/ff/ff.y"
	{ ffn_free((yyvaluep->node)); };

/* Line 1000 of yacc.c  */
#line 1215 "fab/fabd/ff/ff.tab.c"
	break;
      case 31: /* "listparts" */

/* Line 1000 of yacc.c  */
#line 104 "fab/fabd/ff/ff.y"
	{ ffn_free((yyvaluep->node)); };

/* Line 1000 of yacc.c  */
#line 1224 "fab/fabd/ff/ff.tab.c"
	break;
      case 32: /* "listpart" */

/* Line 1000 of yacc.c  */
#line 104 "fab/fabd/ff/ff.y"
	{ ffn_free((yyvaluep->node)); };

/* Line 1000 of yacc.c  */
#line 1233 "fab/fabd/ff/ff.tab.c"
	break;
      case 33: /* "varref" */

/* Line 1000 of yacc.c  */
#line 104 "fab/fabd/ff/ff.y"
	{ ffn_free((yyvaluep->node)); };

/* Line 1000 of yacc.c  */
#line 1242 "fab/fabd/ff/ff.tab.c"
	break;
      case 34: /* "transform" */

/* Line 1000 of yacc.c  */
#line 104 "fab/fabd/ff/ff.y"
	{ ffn_free((yyvaluep->node)); };

/* Line 1000 of yacc.c  */
#line 1251 "fab/fabd/ff/ff.tab.c"
	break;
      case 35: /* "transformparts" */

/* Line 1000 of yacc.c  */
#line 104 "fab/fabd/ff/ff.y"
	{ ffn_free((yyvaluep->node)); };

/* Line 1000 of yacc.c  */
#line 1260 "fab/fabd/ff/ff.tab.c"
	break;
      case 36: /* "nofile" */

/* Line 1000 of yacc.c  */
#line 104 "fab/fabd/ff/ff.y"
	{ ffn_free((yyvaluep->node)); };

/* Line 1000 of yacc.c  */
#line 1269 "fab/fabd/ff/ff.tab.c"
	break;
      case 37: /* "nofileparts" */

/* Line 1000 of yacc.c  */
#line 104 "fab/fabd/ff/ff.y"
	{ ffn_free((yyvaluep->node)); };

/* Line 1000 of yacc.c  */
#line 1278 "fab/fabd/ff/ff.tab.c"
	break;
      case 38: /* "word" */

/* Line 1000 of yacc.c  */
#line 104 "fab/fabd/ff/ff.y"
	{ ffn_free((yyvaluep->node)); };

/* Line 1000 of yacc.c  */
#line 1287 "fab/fabd/ff/ff.tab.c"
	break;
      case 39: /* "statement" */

/* Line 1000 of yacc.c  */
#line 104 "fab/fabd/ff/ff.y"
	{ ffn_free((yyvaluep->node)); };

/* Line 1000 of yacc.c  */
#line 1296 "fab/fabd/ff/ff.tab.c"
	break;
      case 40: /* "onceblock" */

/* Line 1000 of yacc.c  */
#line 104 "fab/fabd/ff/ff.y"
	{ ffn_free((yyvaluep->node)); };

/* Line 1000 of yacc.c  */
#line 1305 "fab/fabd/ff/ff.tab.c"
	break;
      case 41: /* "varassign" */

/* Line 1000 of yacc.c  */
#line 104 "fab/fabd/ff/ff.y"
	{ ffn_free((yyvaluep->node)); };

/* Line 1000 of yacc.c  */
#line 1314 "fab/fabd/ff/ff.tab.c"
	break;
      case 42: /* "invocation" */

/* Line 1000 of yacc.c  */
#line 104 "fab/fabd/ff/ff.y"
	{ ffn_free((yyvaluep->node)); };

/* Line 1000 of yacc.c  */
#line 1323 "fab/fabd/ff/ff.tab.c"
	break;
      case 43: /* "varsettings" */

/* Line 1000 of yacc.c  */
#line 104 "fab/fabd/ff/ff.y"
	{ ffn_free((yyvaluep->node)); };

/* Line 1000 of yacc.c  */
#line 1332 "fab/fabd/ff/ff.tab.c"
	break;
      case 44: /* "varsetting" */

/* Line 1000 of yacc.c  */
#line 104 "fab/fabd/ff/ff.y"
	{ ffn_free((yyvaluep->node)); };

/* Line 1000 of yacc.c  */
#line 1341 "fab/fabd/ff/ff.tab.c"
	break;
      case 45: /* "varlock" */

/* Line 1000 of yacc.c  */
#line 104 "fab/fabd/ff/ff.y"
	{ ffn_free((yyvaluep->node)); };

/* Line 1000 of yacc.c  */
#line 1350 "fab/fabd/ff/ff.tab.c"
	break;
      case 46: /* "varlink" */

/* Line 1000 of yacc.c  */
#line 104 "fab/fabd/ff/ff.y"
	{ ffn_free((yyvaluep->node)); };

/* Line 1000 of yacc.c  */
#line 1359 "fab/fabd/ff/ff.tab.c"
	break;
      case 47: /* "fabrication" */

/* Line 1000 of yacc.c  */
#line 104 "fab/fabd/ff/ff.y"
	{ ffn_free((yyvaluep->node)); };

/* Line 1000 of yacc.c  */
#line 1368 "fab/fabd/ff/ff.tab.c"
	break;
      case 48: /* "discovery" */

/* Line 1000 of yacc.c  */
#line 104 "fab/fabd/ff/ff.y"
	{ ffn_free((yyvaluep->node)); };

/* Line 1000 of yacc.c  */
#line 1377 "fab/fabd/ff/ff.tab.c"
	break;
      case 49: /* "command" */

/* Line 1000 of yacc.c  */
#line 104 "fab/fabd/ff/ff.y"
	{ ffn_free((yyvaluep->node)); };

/* Line 1000 of yacc.c  */
#line 1386 "fab/fabd/ff/ff.tab.c"
	break;
      case 50: /* "commandparts" */

/* Line 1000 of yacc.c  */
#line 104 "fab/fabd/ff/ff.y"
	{ ffn_free((yyvaluep->node)); };

/* Line 1000 of yacc.c  */
#line 1395 "fab/fabd/ff/ff.tab.c"
	break;
      case 51: /* "commandpart" */

/* Line 1000 of yacc.c  */
#line 104 "fab/fabd/ff/ff.y"
	{ ffn_free((yyvaluep->node)); };

/* Line 1000 of yacc.c  */
#line 1404 "fab/fabd/ff/ff.tab.c"
	break;
      case 52: /* "varrefs" */

/* Line 1000 of yacc.c  */
#line 104 "fab/fabd/ff/ff.y"
	{ ffn_free((yyvaluep->node)); };

/* Line 1000 of yacc.c  */
#line 1413 "fab/fabd/ff/ff.tab.c"
	break;

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void* scanner, parse_param* parm);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */





/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void* scanner, parse_param* parm)
#else
int
yyparse (scanner, parm)
    void* scanner;
    parse_param* parm;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[2];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;

#if YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif

/* User initialization code.  */

/* Line 1242 of yacc.c  */
#line 42 "fab/fabd/ff/ff.y"
{ memset(&yylloc, 0, sizeof(yylloc)); }

/* Line 1242 of yacc.c  */
#line 1567 "fab/fabd/ff/ff.tab.c"
  yylsp[0] = yylloc;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 109 "fab/fabd/ff/ff.y"
    {
    YYU_FATAL(ffn_mknode, &parm->ffn, &(yyloc), FFN_STMTLIST, (yyvsp[(1) - (1)].node));
  ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 113 "fab/fabd/ff/ff.y"
    {
    YYU_FATAL(ffn_mknode, &parm->ffn, &(yyloc), FFN_STMTLIST, (void*)0);
	;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 120 "fab/fabd/ff/ff.y"
    {
    (yyval.node) = ffn_addchain((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
  ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 127 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_DEPENDENCY, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), FFN_SINGLE);
	;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 131 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_DEPENDENCY, (yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node), FFN_SINGLE | FFN_WEAK);
	;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 135 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_DEPENDENCY, (yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node), FFN_SINGLE | FFN_BRIDGE);
	;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 139 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_DEPENDENCY, (yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node), FFN_MULTI);
	;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 143 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_DEPENDENCY, (yyvsp[(1) - (5)].node), (yyvsp[(5) - (5)].node), FFN_MULTI | FFN_WEAK);
	;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 147 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_DEPENDENCY, (yyvsp[(1) - (5)].node), (yyvsp[(5) - (5)].node), FFN_MULTI | FFN_BRIDGE);
	;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 153 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_LIST, (yyvsp[(2) - (3)].node)      , (void*)0, (void*)0);
	;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 157 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_LIST, (void*)0, (void*)0, (void*)0);
	;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 161 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_LIST, (yyvsp[(2) - (5)].node)      , (yyvsp[(4) - (5)].node)      , (void*)0);
	;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 165 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_LIST, (void*)0, (yyvsp[(3) - (4)].node), (void*)0);
	;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 169 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_LIST, (yyvsp[(2) - (5)].node)      , (void*)0, (yyvsp[(4) - (5)].node));
	;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 173 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_LIST, (void*)0, (void*)0, (yyvsp[(3) - (4)].node));
	;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 180 "fab/fabd/ff/ff.y"
    {
		(yyval.node) = ffn_addchain((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
	;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 195 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_VARREF, (yyvsp[(2) - (2)].node), (void*)0);
	;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 199 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_VARREF, (yyvsp[(3) - (3)].node), FFN_SYSVAR);
	;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 206 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_TRANSFORM, (yyvsp[(1) - (1)].node));
	;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 213 "fab/fabd/ff/ff.y"
    {
		(yyval.node) = ffn_addchain((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
	;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 221 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_NOFILE, (yyvsp[(2) - (2)].node));
	;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 228 "fab/fabd/ff/ff.y"
    {
		(yyval.node) = ffn_addchain((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
	;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 236 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_WORD);
	;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 240 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_LF);
	;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 255 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_ONCEBLOCK, (yyvsp[(3) - (4)].node));
	;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 262 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_VARASSIGN, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
	;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 269 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_INVOCATION, (yyvsp[(2) - (2)].node), (void*)0, (void*)0, (void*)0);
	;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 273 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_INVOCATION, (yyvsp[(2) - (3)].node), (void*)0, (yyvsp[(3) - (3)].node), (void*)0);
	;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 277 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_INVOCATION, (yyvsp[(2) - (4)].node), (void*)0, (void*)0, FFN_SUBCONTEXT);
	;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 281 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_INVOCATION, (yyvsp[(2) - (5)].node), (void*)0, (yyvsp[(5) - (5)].node), FFN_SUBCONTEXT);
	;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 285 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_INVOCATION, (yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node), (void*)0, FFN_SUBCONTEXT);
	;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 289 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_INVOCATION, (yyvsp[(2) - (6)].node), (yyvsp[(4) - (6)].node), (yyvsp[(6) - (6)].node), FFN_SUBCONTEXT);
	;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 296 "fab/fabd/ff/ff.y"
    {
		(yyval.node) = ffn_addchain((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
	;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 309 "fab/fabd/ff/ff.y"
    {
		/* list assignment */
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_VARLOCK, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
	;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 314 "fab/fabd/ff/ff.y"
    {
		/* cross-name alias */
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_VARLOCK, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
	;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 319 "fab/fabd/ff/ff.y"
    {
		/* same-name alias */
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_VARLOCK, (yyvsp[(1) - (1)].node), (void*)0);
	;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 327 "fab/fabd/ff/ff.y"
    {
		/* cross-name link */
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_VARLINK, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
	;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 332 "fab/fabd/ff/ff.y"
    {
		/* same-name link */
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_VARLINK, (yyvsp[(2) - (2)].node), (void*)0);
	;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 340 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_FORMULA, (void*)0, (yyvsp[(1) - (4)].node)->needs, (yyvsp[(3) - (4)].node), (yyvsp[(1) - (4)].node)->flags | FFN_FABRICATION);
		(yyval.node) = ffn_addchain((yyvsp[(1) - (4)].node), (yyval.node));
	;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 345 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_FORMULA, (yyvsp[(1) - (4)].node), (void*)0, (yyvsp[(3) - (4)].node), FFN_SINGLE | FFN_FABRICATION);
	;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 349 "fab/fabd/ff/ff.y"
    {
		/* this form is redundant but is included for completeness */
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_FORMULA, (yyvsp[(1) - (5)].node), (void*)0, (yyvsp[(4) - (5)].node), FFN_SINGLE | FFN_FABRICATION);
	;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 354 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_FORMULA, (yyvsp[(1) - (6)].node), (void*)0, (yyvsp[(5) - (6)].node), FFN_MULTI | FFN_FABRICATION);
	;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 361 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_FORMULA, (yyvsp[(1) - (5)].node), (void*)0, (yyvsp[(4) - (5)].node), FFN_SINGLE | FFN_DISCOVERY);
	;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 368 "fab/fabd/ff/ff.y"
    {
		YYU_FATAL(ffn_mknode, &(yyval.node), &(yyloc), FFN_LIST, (yyvsp[(1) - (1)].node), (void*)0, (void*)0);
	;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 375 "fab/fabd/ff/ff.y"
    {
		(yyval.node) = ffn_addchain((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
	;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 388 "fab/fabd/ff/ff.y"
    {
		(yyval.node) = ffn_addchain((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node));
	;}
    break;



/* Line 1455 of yacc.c  */
#line 2176 "fab/fabd/ff/ff.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, scanner, parm, YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (&yylloc, scanner, parm, yymsg);
	  }
	else
	  {
	    yyerror (&yylloc, scanner, parm, YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc, scanner, parm);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[0] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp, scanner, parm);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, scanner, parm, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc, scanner, parm);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp, scanner, parm);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



