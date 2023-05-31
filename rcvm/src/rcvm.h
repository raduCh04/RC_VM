#pragma once

#include "utils.h"
#include "error.h"
#include "io.h"

typedef enum radu_chira_virtual_machine_registers
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
} rcvm_registers_t;

typedef enum radu_chira_virtual_machine_register_flags
{
	FLAGS_ZERO = 0x40,
	FLAGS_SIGNED = 0x80,
} rcvm_registers_flags_t;

typedef struct radu_chira_virtual_machine rcvm_t;


RCVM_API rcvm_t *rcvm_init(void);
RCVM_API void rcvm_destroy(rcvm_t *vm);

RCVM_API uint8 rcvm_mem_read(rcvm_t *vm, uint16 address);
RCVM_API void rcvm_mem_write(rcvm_t *vm ,uint16 address, uint8);
RCVM_API uint16 rcvm_reg_read(rcvm_t *vm, rcvm_registers_t reg);
RCVM_API void rcvm_reg_write(rcvm_t *vm, rcvm_registers_t reg, uint16 value);

RCVM_API void rcvm_load_program_data(rcvm_t *vm, usize size, uint8 *data);
RCVM_API void rcvm_load_program_from_file(rcvm_t *vm, usize *size, const char *path);

RCVM_API void rcvm_execute_program(rcvm_t *vm, usize program_size);

RCVM_API void rcvm_print_regs(rcvm_t *vm);
RCVM_API void rcvm_print_memory(rcvm_t *vm);
RCVM_API void rcvm_print_memory_from_to(rcvm_t *vm, usize lower, usize upper);

