#ifndef _LOG_H
#define _LOG_H

#include <stdarg.h>
#include <stdint.h>

// C
#define L_COLOR_VALUE   0x0000000F00000000ULL // C range

#define L_RED           0x0000000100000000ULL // this message red in the terminal
#define L_GREEN         0x0000000200000000ULL // this message green in the terminal
#define L_YELLOW        0x0000000300000000ULL // this message yellow in the terminal
#define L_CYAN          0x0000000400000000ULL // this message cyan in the terminal
#define L_BLUE          0x0000000500000000ULL // this message blue in the terminal

// E
#define L_TAG           0x000000003FFFFFFFULL  // D range

#define L_ERROR         0x00000001ULL | L_RED			/* errors leading to shutdown */
#define L_WARN          0x00000002ULL | L_YELLOW	/* nonfatal warnings */
#define L_INFO          0x00000004ULL							/* program flow */
#define L_ARGS 					0x00000008ULL							/* program arguments */
#define L_FFTOKN 				0x00000020ULL							/* fabfile parsing - token stream */
#define L_FFSTAT 				0x00000040ULL							/* fabfile parsing - lexer start condition changes*/
#define L_FFTREE				0x00000080ULL							/* fabfile parsing - parsed tree */
#define L_FF						0x00000010ULL							/* fabfile */
#define L_BPEXEC				0x00000200ULL							/* buildplan - execution */
#define L_BPEVAL				0x00000400ULL							/* buildplan - pruning/evaluating */
#define L_BPDUMP				0x00000800ULL							/* buildplan - dump the final buildplan */
#define L_BP						0x00000100ULL							/* buildplan */
#define L_FMLEXEC				0x00002000ULL							/* formulas - execution results/details */
#define L_FMLTARG				0x00004000ULL							/* formulas - target resolution/assignment */
#define L_FML						0x00001000ULL							/* formulas */
#define L_DGRAPH 				0x00008000ULL							/* dependency graph - dump/details */
#define L_DGHASH				0x00010000ULL							/* dependency graph - hash loading/saving */
#define L_DG 						0x00008000ULL							/* dependency graph */
#define L_VAR						0x00020000ULL							/* variable defintions */
#define L_LWDEBUG				0x00040000ULL							/* debug liblistwise invocations */

/// log_init
//
// initialize logging, parse cmdline args as well as {args}
//
int log_init(char * args);

// log_parse
//
// parse the log string to enable/disable tags
//
void log_parse(char * args, int args_len);

/// log_active
//
// write a string describing active logging categories to s
//
void log_active(char* s, size_t l);

/// log_would
//
// true if logs would print with the specified bits
//
int log_would(const uint64_t bits);

/// log
//
// writes messages to stderr if log_would([bits])
//
void log(const uint64_t bits, const char* fmt, ...)
	__attribute__((nonnull));

/// log_start
//
// begin writing a single log
//
void log_start(const uint64_t bits, const char* fmt, ...)
	__attribute__((nonnull));

/// log_finish
//
// complete the log begun with log_start
//
void log_finish(const char* fmt, ...);

/// log_teardown
//
// cleanup
//
void log_teardown();

#endif
