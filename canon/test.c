#include <stdio.h>
#include <unistd.h>

#include "canon.h"

int main(int argc, char ** argv)
{
	printf("in : '%s'\n", argv[1]);

	char out[512];
	char cwd[512];

	uint32_t opts = 0;
	if(argc > 2)
		sscanf(argv[2], "%u", &opts);

	if(canon(argv[1], 0, out, sizeof(out), getcwd(cwd, sizeof(cwd)), opts) == 0)
		printf("canon failed\n");

	printf("out: '%s'\n", out);

	return 0;
}
