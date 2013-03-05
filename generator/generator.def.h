#ifndef _GENERATOR_DEF_H
#define _GENERATOR_DEF_H

#include <stdio.h>
#include <stdint.h>

#include "listwise/internal.h"

typedef struct
{
	int							r;			// return value of the parse
	void*						scanner;

	int							argsa;
	int							opsa;
	int							opargsa;

	generator*			g;			// completed generator goes here
} parse_param;

#endif

