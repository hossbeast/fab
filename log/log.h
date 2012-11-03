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
#define L_BPEXEC				0x00000008ULL							/* buildplan execution */
#define L_BPEVAL				0x00000010ULL							/* buildplan evaluation results */
#define L_BPDUMP				0x00000020ULL							/* dump the final buildplan */
#define L_FFTOK 				0x00000040ULL							/* fabfile parsing, token stream */
#define L_FF						0x00000080ULL							/* fabfile parsing */
#define L_ARGS 					0x00000100ULL							/* program arguments */
#define L_GN 						0x00000200ULL							/* graph node details */
#define L_FMEXEC				0x00000400ULL							/* formula execution results/details */
#define L_HASH					0x00000800ULL							/* hash loading/saving on per-file basis */

/// log_init
//
// initialize logging, parse cmdline args as well as {args}
//
int log_init(char * args);

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

#endif
