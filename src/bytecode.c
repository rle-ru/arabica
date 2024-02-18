#include "bytecode.h"
#include <stdlib.h>
#include "code.h"
#include <stdio.h>
#include <string.h>

int bytecode_load_val(code *c, unsigned char *a)
{
	int value = *((int *)a);
	value = reverse_endian_32(value);
	if (c->verbose)
		printf("LOAD_VAL %d\n", value);
	push_stack(&c->stack, TYPE_INT, (unsigned char *)&value);
	c->position += 4;
	return (0);
}

int bytecode_read_var(code *c, unsigned char *a)
{
	(void)a;
	unsigned char index = a[0];
	push_stack(&c->stack, c->registers[index].type, (unsigned char *)&c->registers[index]);
	if (c->verbose)
	{
		if (c->registers[index].type == TYPE_CSTRING)
			printf("READ_VAR in register %d : %s\n", index, *((char **)c->registers[index].data));
		if (c->registers[index].type == TYPE_DSTRING)
			printf("READ_VAR in register %d : %s\n", index, *((char **)c->registers[index].data));
		if (c->registers[index].type == TYPE_INT)
			printf("READ_VAR in register %d : %d\n", index, *((int *)c->registers[index].data));
	}
	c->position += 1;
	return (0);
}

int bytecode_store_var(code *c, unsigned char *a)
{
	unsigned char index = a[0];
	t_data data = pop_stack(&c->stack);
	c->registers[index] = data;
	if (c->verbose)
	{
		if (data.type == TYPE_CSTRING)
			printf("STORE_VAR in register %d : %s\n", index, *((char **)data.data));
		if (data.type == TYPE_DSTRING)
			printf("STORE_VAR in register %d : %s\n", index, *((char **)data.data));
		if (data.type == TYPE_INT)
			printf("STORE_VAR in register %d : %d\n", index, *((int *)data.data));
	}
	c->position += 1;
	return (0);
}

int bytecode_add(code *c, unsigned char *z)
{
	(void)z;
	t_data b = pop_stack(&c->stack);
	t_data a = pop_stack(&c->stack);
	if (a.type != TYPE_INT || b.type != TYPE_INT)
	{
		printf("Error: expected int on stack\n");
		return (1);
	}
	int result = *((int *)a.data) + *((int *)b.data);
	push_stack(&c->stack, TYPE_INT, (unsigned char *)&result);
	if (c->verbose)
		printf("ADD %d + %d = %d\n", *((int *)a.data), *((int *)b.data), result);
	return (0);
}

int bytecode_sub(code *c, unsigned char *z)
{
	(void)z;
	t_data b = pop_stack(&c->stack);
	t_data a = pop_stack(&c->stack);
	if (a.type != TYPE_INT || b.type != TYPE_INT)
	{
		printf("Error: expected int on stack\n");
		return (1);
	}
	int result = *((int *)a.data) - *((int *)b.data);
	push_stack(&c->stack, TYPE_INT, (unsigned char *)&result);
	if (c->verbose)
		printf("SUB %d - %d = %d\n", *((int *)a.data), *((int *)b.data), result);
	return (0);
}

int bytecode_mul(code *c, unsigned char *z)
{
	(void)z;
	t_data b = pop_stack(&c->stack);
	t_data a = pop_stack(&c->stack);
	if (a.type != TYPE_INT || b.type != TYPE_INT)
	{
		printf("Error: expected int on stack\n");
		return (1);
	}
	int result = *((int *)a.data) * *((int *)b.data);
	push_stack(&c->stack, TYPE_INT, (unsigned char *)&result);
	if (c->verbose)
		printf("MUL %d * %d = %d\n", *((int *)a.data), *((int *)b.data), result);
	return (0);
}

int bytecode_div(code *c, unsigned char *z)
{
	(void)z;
	t_data b = pop_stack(&c->stack);
	t_data a = pop_stack(&c->stack);
	if (a.type != TYPE_INT || b.type != TYPE_INT)
	{
		printf("Error: expected int on stack\n");
		return (1);
	}
	if (*((int *)b.data) == 0)
	{
		printf("Error: division by zero\n");
		return (1);
	}
	int result = *((int *)a.data) / *((int *)b.data);
	push_stack(&c->stack, TYPE_INT, (unsigned char *)&result);
	if (c->verbose)
		printf("DIV %d / %d = %d\n", *((int *)a.data), *((int *)b.data), result);
	return (0);
}

int bytecode_mod(code *c, unsigned char *z)
{
	(void)z;
	t_data b = pop_stack(&c->stack);
	t_data a = pop_stack(&c->stack);
	if (a.type != TYPE_INT || b.type != TYPE_INT)
	{
		printf("Error: expected int on stack\n");
		return (1);
	}
	if (*((int *)b.data) == 0)
	{
		printf("Error: division by zero\n");
		return (1);
	}
	int result = *((int *)a.data) % *((int *)b.data);
	push_stack(&c->stack, TYPE_INT, (unsigned char *)&result);
	if (c->verbose)
		printf("MOD %d %% %d = %d\n", *((int *)a.data), *((int *)b.data), result);
	return (0);
}

int bytecode_jmp(code *c, unsigned char *a)
{
	int offset = *((int *)a);
	offset = reverse_endian_32(offset);
	c->position += offset;
	if (c->verbose)
		printf("JMP %d\n", offset);
	return (0);
}

int bytecode_jmp_if_zero(code *c, unsigned char *a)
{
	int offset = *((int *)a);
	offset = reverse_endian_32(offset);
	t_data data = pop_stack(&c->stack);
	if (data.type != TYPE_INT)
	{
		printf("Error: expected int on stack\n");
		return (1);
	}
	if (c->verbose)
	{
		if (*((int *)data.data) == 0)
			printf("JMP_IF_ZERO %d\n", offset);
		else
			printf("JMP_IF_ZERO %d\n", 4);
	}
	if (*((int *)data.data) == 0)
		c->position += offset;
	else
		c->position += 4;
	return (0);
}

int bytecode_jmp_if_true(code *c, unsigned char *a)
{
	int offset = *((int *)a);
	offset = reverse_endian_32(offset);
	t_data data = pop_stack(&c->stack);
	if (data.type != TYPE_INT)
	{
		printf("Error: expected int on stack\n");
		return (1);
	}
	if (c->verbose)
	{
		if (*((int *)data.data) != 0)
			printf("JMP_IF_TRUE %d\n", offset);
		else
			printf("JMP_IF_TRUE %d\n", 4);
	}
	if (*((int *)data.data) != 0)
		c->position += offset;
	else
		c->position += 4;
	return (0);
}

int bytecode_eq(code *c, unsigned char *z)
{
	(void)z;
	t_data a = pop_stack(&c->stack);
	t_data b = pop_stack(&c->stack);
	if (a.type != TYPE_INT || b.type != TYPE_INT)
	{
		printf("Error: expected int on stack\n");
		return (1);
	}
	int result = *((int *)a.data) == *((int *)b.data);
	push_stack(&c->stack, TYPE_INT, (unsigned char *)&result);
	if (c->verbose)
		printf("EQ %d == %d = %d\n", *((int *)a.data), *((int *)b.data), result);
	return (0);
}

int bytecode_neq(code *c, unsigned char *z)
{
	(void)z;
	t_data a = pop_stack(&c->stack);
	t_data b = pop_stack(&c->stack);
	if (a.type != TYPE_INT || b.type != TYPE_INT)
	{
		printf("Error: expected int on stack\n");
		return (1);
	}
	int result = *((int *)a.data) != *((int *)b.data);
	push_stack(&c->stack, TYPE_INT, (unsigned char *)&result);
	if (c->verbose)
		printf("NEQ %d != %d = %d\n", *((int *)a.data), *((int *)b.data), result);
	return (0);
}

int bytecode_gt(code *c, unsigned char *z)
{
	(void)z;
	t_data a = pop_stack(&c->stack);
	t_data b = pop_stack(&c->stack);
	if (a.type != TYPE_INT || b.type != TYPE_INT)
	{
		printf("Error: expected int on stack\n");
		return (1);
	}
	int result = *((int *)a.data) < *((int *)b.data);
	push_stack(&c->stack, TYPE_INT, (unsigned char *)&result);
	if (c->verbose)
		printf("GT %d > %d = %d\n", *((int *)a.data), *((int *)b.data), result);
	return (0);
}

int bytecode_lt(code *c, unsigned char *z)
{
	(void)z;
	t_data a = pop_stack(&c->stack);
	t_data b = pop_stack(&c->stack);
	if (a.type != TYPE_INT || b.type != TYPE_INT)
	{
		printf("Error: expected int on stack\n");
		return (1);
	}
	int result = *((int *)a.data) > *((int *)b.data);
	push_stack(&c->stack, TYPE_INT, (unsigned char *)&result);
	if (c->verbose)
		printf("LT %d < %d = %d\n", *((int *)a.data), *((int *)b.data), result);
	return (0);
}

int bytecode_gte(code *c, unsigned char *z)
{
	(void)z;
	t_data a = pop_stack(&c->stack);
	t_data b = pop_stack(&c->stack);
	if (a.type != TYPE_INT || b.type != TYPE_INT)
	{
		printf("Error: expected int on stack\n");
		return (1);
	}
	int result = *((int *)a.data) <= *((int *)b.data);
	push_stack(&c->stack, TYPE_INT, (unsigned char *)&result);
	if (c->verbose)
		printf("GTE %d >= %d = %d\n", *((int *)a.data), *((int *)b.data), result);
	return (0);
}

int bytecode_lte(code *c, unsigned char *z)
{
	(void)z;
	t_data a = pop_stack(&c->stack);
	t_data b = pop_stack(&c->stack);
	if (a.type != TYPE_INT || b.type != TYPE_INT)
	{
		printf("Error: expected int on stack\n");
		return (1);
	}
	int result = *((int *)a.data) >= *((int *)b.data);
	push_stack(&c->stack, TYPE_INT, (unsigned char *)&result);
	if (c->verbose)
		printf("LTE %d <= %d = %d\n", *((int *)a.data), *((int *)b.data), result);
	return (0);
}

int bytecode_print_val(code *c, unsigned char *a)
{
	(void)a;
	t_data data = pop_stack(&c->stack);
	if (data.type != TYPE_INT)
	{
		printf("Error: expected int on stack\n");
		return (1);
	}
	if (c->verbose)
		printf("PRINT_VAL %d\n", *((int *)data.data));
	printf("%d", *((int *)data.data));
	return (0);
}

int bytecode_input_val(code *c, unsigned char *a)
{
	(void)a;
	int value;
	scanf("%d", &value);
	push_stack(&c->stack, TYPE_INT, (unsigned char *)&value);
	if (c->verbose)
		printf("INPUT_VAL %d\n", value);
	return (0);
}

int bytecode_halt(code *c, unsigned char *a)
{
	(void)a;
	c->running = 0;
	if (c->verbose)
		printf("HALT\n");
	return (0);
}

int bytecode_load_str(code *c, unsigned char *a)
{
	int len = a[0];
	unsigned char *str = malloc(len + 1);
	if (!str)
		return (1);
	memcpy(str, &a[1], len);
	str[len] = 0;
	push_stack(&c->stack, TYPE_DSTRING, (unsigned char *)&str);
	c->position += 1 + len;
	if (c->verbose)
		printf("LOAD_STR %s\n", str);
	return (0);
}

int bytecode_print_str(code *c, unsigned char *a)
{
	(void)a;
	t_data data = pop_stack(&c->stack);
	if (data.type != TYPE_CSTRING && data.type != TYPE_DSTRING)
	{
		printf("Error: expected string on stack\n");
		return (1);
	}
	if (c->verbose)
		printf("PRINT_STR %s\n", *((char **)data.data));
	printf("%s", *((char **)data.data));
	return (0);
}

int bytecode_input_str(code *c, unsigned char *a)
{
	(void)a;
	printf("input_str\n");
	char buffer[1024];
	ssize_t read = scanf("%1024s", buffer);
	if (read == EOF)
	{
		printf("Error: failed to read line\n");
		return (1);
	}
	char *str = strdup(buffer);
	if (!str)
		return (1);
	push_stack(&c->stack, TYPE_DSTRING, (unsigned char *)&str);
	if (c->verbose)
		printf("INPUT_STR %s\n", str);
	return (0);
}

int bytecode_str_len(code *c, unsigned char *a)
{
	(void)a;
	t_data data = pop_stack(&c->stack);
	if (data.type != TYPE_CSTRING && data.type != TYPE_DSTRING)
	{
		printf("Error: expected string on stack\n");
		return (1);
	}
	int len = strlen(*((char **)data.data));
	push_stack(&c->stack, TYPE_INT, (unsigned char *)&len);
	if (c->verbose)
		printf("STR_LEN %d\n", len);
	return (0);
}

int bytecode_concat(code *c, unsigned char *z)
{
	(void)z;
	t_data a = pop_stack(&c->stack);
	t_data b = pop_stack(&c->stack);
	if (a.type != TYPE_CSTRING && a.type != TYPE_DSTRING)
	{
		printf("Error: expected string on stack\n");
		return (1);
	}
	if (b.type != TYPE_CSTRING && b.type != TYPE_DSTRING)
	{
		printf("Error: expected string on stack\n");
		return (1);
	}
	int len = strlen(*((char **)a.data)) + strlen(*((char **)b.data));
	char *str = malloc(len + 1);
	if (!str)
		return (1);
	strcpy(str, *((char **)a.data));
	strcat(str, *((char **)b.data));
	push_stack(&c->stack, TYPE_DSTRING, (unsigned char *)&str);
	if (c->verbose)
		printf("CONCAT %s + %s = %s\n", *((char **)a.data), *((char **)b.data), str);
	return (0);
}

int bytecode_get_char(code *c, unsigned char *z)
{
	(void)z;
	t_data str = pop_stack(&c->stack);
	t_data index = pop_stack(&c->stack);
	if (str.type != TYPE_CSTRING && str.type != TYPE_DSTRING)
	{
		printf("Error: expected string on stack\n");
		return (1);
	}
	if (index.type != TYPE_INT)
	{
		printf("Error: expected int on stack\n");
		return (1);
	}
	char *s = *((char **)str.data);
	int i = *((int *)index.data);
	if (i < 0 || (size_t)i >= strlen(s))
	{
		printf("Error: index out of bounds\n");
		return (1);
	}
	int cha = s[i];
	push_stack(&c->stack, TYPE_INT, (unsigned char *)&cha);
	if (c->verbose)
		printf("GET_CHAR %s[%d] = %c\n", s, i, cha);
	return (0);
}

int bytecode_set_char(code *c, unsigned char *a)
{
	(void)a;
	t_data value = pop_stack(&c->stack);
	t_data index = pop_stack(&c->stack);
	t_data str = pop_stack(&c->stack);
	if (str.type != TYPE_CSTRING && str.type != TYPE_DSTRING)
	{
		printf("Error: expected string on stack\n");
		return (1);
	}
	if (index.type != TYPE_INT)
	{
		printf("Error: expected int on stack\n");
		return (1);
	}
	if (value.type != TYPE_INT)
	{
		printf("Error: expected int on stack\n");
		return (1);
	}
	char *s = *((char **)str.data);
	int i = *((int *)index.data);
	if (i < 0 || (size_t)i >= strlen(s))
	{
		printf("Error: index out of bounds\n");
		return (1);
	}
	s[i] = *((char *)value.data);
	if (c->verbose)
		printf("SET_CHAR %s[%d] = %c\n", s, i, *((char *)value.data));
	return (0);
}

int bytecode_str_cmp(code *c, unsigned char *z)
{
	(void)z;
	t_data a = pop_stack(&c->stack);
	t_data b = pop_stack(&c->stack);
	if (a.type != TYPE_CSTRING && a.type != TYPE_DSTRING)
	{
		printf("Error: expected string on stack\n");
		return (1);
	}
	if (b.type != TYPE_CSTRING && b.type != TYPE_DSTRING)
	{
		printf("Error: expected string on stack\n");
		return (1);
	}
	int result = strcmp(*((char **)a.data), *((char **)b.data));
	push_stack(&c->stack, TYPE_INT, (unsigned char *)&result);
	if (c->verbose)
		printf("STR_CMP %s cmp %s = %d\n", *((char **)a.data), *((char **)b.data), result);
	return (0);
}

bytecode *create_bytecodes()
{
	bytecode *bc = malloc(sizeof(bytecode) * 28);
	if (!bc)
		return (NULL);
	bc[0] = (bytecode){.name = "LOAD_VAL", .argc = 4, .func = bytecode_load_val};
	bc[1] = (bytecode){.name = "READ_VAR", .argc = 1, .func = bytecode_read_var};
	bc[2] = (bytecode){.name = "STORE_VAR", .argc = 1, .func = bytecode_store_var};
	bc[3] = (bytecode){.name = "ADD", .argc = 0, .func = bytecode_add};
	bc[4] = (bytecode){.name = "SUB", .argc = 0, .func = bytecode_sub};
	bc[5] = (bytecode){.name = "MUL", .argc = 0, .func = bytecode_mul};
	bc[6] = (bytecode){.name = "DIV", .argc = 0, .func = bytecode_div};
	bc[7] = (bytecode){.name = "MOD", .argc = 0, .func = bytecode_mod};
	bc[8] = (bytecode){.name = "JMP", .argc = 4, .func = bytecode_jmp};
	bc[9] = (bytecode){.name = "JMP_IF_ZERO", .argc = 4, .func = bytecode_jmp_if_zero};
	bc[10] = (bytecode){.name = "JMP_IF_TRUE", .argc = 4, .func = bytecode_jmp_if_true};
	bc[11] = (bytecode){.name = "EQ", .argc = 0, .func = bytecode_eq};
	bc[12] = (bytecode){.name = "NEQ", .argc = 0, .func = bytecode_neq};
	bc[13] = (bytecode){.name = "GT", .argc = 0, .func = bytecode_gt};
	bc[14] = (bytecode){.name = "LT", .argc = 0, .func = bytecode_gt};
	bc[15] = (bytecode){.name = "GTE", .argc = 0, .func = bytecode_gt};
	bc[16] = (bytecode){.name = "LTE", .argc = 0, .func = bytecode_gt};
	bc[17] = (bytecode){.name = "PRINT_VAL", .argc = 0, .func = bytecode_print_val};
	bc[18] = (bytecode){.name = "INPUT_VAL", .argc = 0, .func = bytecode_input_val};
	bc[19] = (bytecode){.name = "HALT", .argc = 0, .func = bytecode_halt};
	bc[20] = (bytecode){.name = "LOAD_STR", .argc = 1, .func = bytecode_load_str};
	bc[21] = (bytecode){.name = "PRINT_STR", .argc = 0, .func = bytecode_print_str};
	bc[22] = (bytecode){.name = "INPUT_STR", .argc = 0, .func = bytecode_input_str};
	bc[23] = (bytecode){.name = "STR_LEN", .argc = 0, .func = bytecode_str_len};
	bc[24] = (bytecode){.name = "CONCAT", .argc = 0, .func = bytecode_concat};
	bc[25] = (bytecode){.name = "GET_CHAR", .argc = 0, .func = bytecode_get_char};
	bc[26] = (bytecode){.name = "SET_CHAR", .argc = 0, .func = bytecode_set_char};
	bc[27] = (bytecode){.name = "STR_CMP", .argc = 0, .func = bytecode_str_cmp};
	return (bc);
}
