#pragma once

#include "utils.h"
#include "error.h"
#include "io.h"

typedef enum registers
{
	REG_A = 0,
	REG_C,
	REG_D,
	REG_B,

	REG_SP,
	REG_BP,
	REG_SI,
	REG_DI,

	REG_IP,
	REG_FLAGS,

	REG_COUNT
} registers_t;

typedef struct virtual_machine vm_t;

vm_t *vm_init(void);
uint8 mem_read(vm_t *vm, uint16 address);
void mem_write(vm_t *vm ,uint16 address, uint8);
uint16 reg_read(vm_t *vm, registers_t reg);
void reg_write(vm_t *vm, registers_t reg, uint16 value);
void load_program_data(vm_t *vm, usize size, uint8 *data);
void load_program_from_file(vm_t *vm, usize *size, const char *path);
void execute_program(vm_t *vm, usize program_size);
void print_regs(vm_t *vm);
void print_memory(vm_t *vm);
void print_memory_from_to(vm_t *vm, usize lower, usize upper);

