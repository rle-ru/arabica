#ifndef BYTECODE_H
#define BYTECODE_H

#include "code.h"

typedef struct
{
	char *name;
	unsigned char argc;
	int (*func)(code *, unsigned char *);
} bytecode;

bytecode *create_bytecodes();
int bytecode_load_val(code *c, unsigned char *a);  // 0x01
int bytecode_read_var(code *c, unsigned char *a);  // 0x02
int bytecode_store_var(code *c, unsigned char *a); // 0x03
int bytecode_add(code *c, unsigned char *a);	   // 0x04
int bytecode_sub(code *c, unsigned char *z);	   // 0x05

int bytecode_load_str(code *c, unsigned char *a);  // 0x15
int bytecode_print_str(code *c, unsigned char *a); // 0x16

// Endian
int reverse_endian_32(int n);

#endif
