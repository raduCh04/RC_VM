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
} registers;

typedef struct vm
{
	uint8 (*mem_read)(uint16 address);
	void (*mem_write)(uint16 address, uint8 value);
	uint16 (*reg_read)(registers reg);
	void (*reg_write)(registers reg, uint16 value);
	void (*load_program_data)(usize size, uint8 *data);
	void (*load_program_from_file)(usize *size, const char *path);
	void (*execute_program)(usize program_size);
	void (*print_regs)(void);
	void (*print_memory)(void);
	void (*print_memory_from_to)(usize lower, usize upper);	
} vm_t;

void vm_init_ptr(vm_t *vm);
vm_t vm_init(void);
