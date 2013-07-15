#include <stdio.h>
#include <unistd.h>

#include "canon.h"

int main(int argc, char ** argv)
{
	char * in = 0;
	char * base = 0;
	uint32_t opts = 0;

	if(argc == 2)
	{
		in = argv[1];
	}
	else if(argc == 3)
	{
		in = argv[1];

		uint32_t u32 = 0;
		int n = 0;
		if(sscanf(argv[2], "%u%n", &u32, &n) == 1 && n)
		{
			opts = u32;
		}
		else
		{
			base = argv[2];
		}
	}
	else if(argc == 4)
	{
		in = argv[1];
		base = argv[2];
		sscanf(argv[3], "%u", &opts);
	}
	else
	{
		dprintf(2, "invalid arguments\n");
		return 1;
	}

	if(base == 0)
	{
		char cwd[512];
		getcwd(cwd, sizeof(cwd));
		base = cwd;
	}

	printf("in   : '%s'\n", in);
	printf("base : '%s'\n", base);
	printf("opts : %u\n", opts);

	char out[512];

	if(canon(in, 0, out, sizeof(out), base, opts) == 0)
		printf("canon failed\n");

	printf("out: '%s'\n", out);

	return 0;
}
