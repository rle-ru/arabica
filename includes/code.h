#ifndef CODE_H
#define CODE_H

#define MAGIC "CODE"

typedef enum type
{
	TYPE_INT,
	TYPE_CSTRING,
	TYPE_DSTRING
} e_type;

typedef struct data
{
	unsigned char data[8];
	e_type type;
} t_data;

typedef struct stack
{
	t_data data;
	struct stack *next;
} t_stack;

typedef struct header
{
	char magic[4];
	unsigned int size;
	unsigned char name[16];
} header;

typedef struct code
{
	header header;
	unsigned char *code;
	unsigned int position;
	t_stack *stack;
	t_data registers[256];
	int running;
	int verbose;
} code;

int code_load(code *code, char *filename);
void push_stack(t_stack **stack, e_type type, unsigned char *data);
t_data pop_stack(t_stack **stack);

#endif
