#include <stdio.h>
#include <stdlib.h>
#include "code.h"
#include "bytecode.h"
#include <string.h>

int main(int ac, char **av)
{
	if (ac < 2)
	{
		printf("Usage: %s <file> [-v]\n", av[0]);
		return (0);
	}
	code c;
	if (ac == 3 && strcmp(av[2], "-v") == 0)
		c.verbose = 1;
	else
		c.verbose = 0;
	int ret = code_load(&c, av[1]);
	if (ret)
		return (1);

	bytecode *bc = create_bytecodes();
	if (!bc)
	{
		printf("Error: failed to allocate memory for bytecodes\n");
		return (1);
	}
	printf("Magic: %.4s\n", c.header.magic);
	printf("Size: %u\n", c.header.size);
	printf("Program name : %.16s\n", c.header.name);

	while (c.running)
	{
		unsigned char opcode = c.code[c.position];
		if (c.verbose)
			printf("Opcode: %x\nPosition: %d\n", opcode, c.position);
		if (opcode == 0 || c.position >= c.header.size)
			c.running = 0;
		else if (opcode > 28)
		{
			c.running = 0;
			printf("Error: invalid opcode %x\n", opcode);
			break ;
		}
		else
		{
			opcode--;
			if (c.verbose)
				printf("Executing %s\n", bc[opcode].name);
			bytecode *b = &bc[opcode];
			unsigned char *args = c.code + c.position + 1;
			if (b->func(&c, args))
				c.running = 0;
			c.position += 1;
		}
	}
	t_stack *s = c.stack;
	while (s)
	{
		// if (s->data.type == TYPE_DSTRING)
		// 	free(s->data.data);
		t_stack *tmp = s;
		s = s->next;
		free(tmp);
	}
	free(bc);
	free(c.code);
	return (0);
}
