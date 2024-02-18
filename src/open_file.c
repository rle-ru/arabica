#include "code.h"
#include "bytecode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int code_load(code *c, char *path)
{
	FILE *file;
	file = fopen(path, "r");
	if (!file)
	{
		printf("Error: could not open file %s\n", path);
		fclose(file);
		return (1);
	}
	size_t ret = fread(c, sizeof(header), 1, file);
	if (ret != 1)
	{
		printf("Error: could not read header from file %s\n", path);
		fclose(file);
		return (1);
	}
	c->header.size = reverse_endian_32(c->header.size);
	if (strncmp(c->header.magic, MAGIC, 4) != 0)
	{
		printf("Error: file %s is not a code file\n", path);
		fclose(file);
		return (1);
	}
	c->code = malloc(c->header.size);
	if (!c->code)
	{
		printf("Error: could not allocate memory for code\n");
		fclose(file);
		return (1);
	}
	ret = fread(c->code, c->header.size, 1, file);
	if (ret != 1)
	{
		printf("Error: could not read code from file %s (incorrect size ? expected %u)\n", path, c->header.size);
		fclose(file);
		return (1);
	}
	c->position = 0;
	c->stack = NULL;
	c->running = 1;
	fclose(file);
	return (0);
}
