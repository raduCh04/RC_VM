#pragma once

#define ASSERT(expr, ...) \
	{ if ((expr)) {} \
		else { fprintf(stderr, "Assertion failure! Expr: %s, Line: %d, File: %s\n", #expr, __LINE__, __FILE__);\
		   	fprintf(stderr, __VA_ARGS__); exit(1); } }

#define MEM_SIZE ((1 << 16) * 2)
#define MEM_OFFSET 512 // for not writing to the initial program

typedef enum reg
{
	REG_A, // accumulator
	REG_C, // count
	REG_D, // data
	REG_B, // base

	REG_SP, // stack pointer
	REG_BP, // base pointer
	REG_SI, // source index
	REG_DI, // destination index

	REG_IP, // Instruction pointer
	REG_FLAGS, // Flags (signed, zero ...)

	REG_COUNT
} reg;

typedef enum flags
{
	FLAG_ZERO = 0x40,
	FLAG_SIGN = 0x80,
} flags;


