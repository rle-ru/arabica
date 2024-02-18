#include "code.h"
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

void push_stack(t_stack **stack, e_type type, unsigned char *data)
{
	t_stack *previous = *stack;
	t_stack *new = malloc(sizeof(t_stack));
	new->data.type = type;
	memcpy(new->data.data, data, 8);
	new->next = previous;
	*stack = new;
}

t_data pop_stack(t_stack **stack)
{
	t_stack *previous = *stack;
	t_data data = previous->data;
	*stack = previous->next;
	free(previous);
	return (data);
}
