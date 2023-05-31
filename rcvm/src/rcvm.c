#include "rcvm.h"
#include <stdbool.h>
#include <string.h>
#include "instructions.h"

#define MEMORY_MAX ((1 << 16) * 2)
#define MEMORY_OFFSET 0x159F

struct radu_chira_virtual_machine
{
	uint8 memory[MEMORY_MAX];
	uint16 registers[REG_COUNT];	
};

inline uint8 rcvm_mem_read(rcvm_t *vm, uint16 address)
{
	//TODO: Handle error
	return (vm->memory[address + MEMORY_OFFSET]);
}

inline void rcvm_mem_write(rcvm_t *vm, uint16 address, uint8 value)
{
	//TODO: Handle error
	vm->memory[address + MEMORY_OFFSET] = value;	
}

inline uint16 rcvm_reg_read(rcvm_t *vm, rcvm_registers_t reg)
{
	//TODO: Handle error
	return (vm->registers[reg]);
}

inline void rcvm_reg_write(rcvm_t *vm, rcvm_registers_t reg, uint16 value)
{
	//TODO: Handle error
	vm->registers[reg] = value;

}

void rcvm_load_program_data(rcvm_t *vm, usize size, uint8 *data)
{
	//TODO: Handle error and add secure memcpy
	memcpy(vm->memory, data, size);
}

void rcvm_load_program_from_file(rcvm_t *vm, usize *size, const char *path)
{
	//TODO: Handle error and add secure memcpy
	uint8 *data = file_read_binary(path, size);
   	memcpy(vm->memory, data, *size);	
}

void rcvm_execute_program(rcvm_t *vm, usize program_size)
{
	printf("bits 16\n");
	for (uint8 *byte = vm->memory;
		byte != &vm->memory[program_size];
		byte = vm->memory + vm->registers[REG_IP])
	{
		uint8 instr = *byte;
		if ((instr >> 2) == MOV_REG_MEM)
		{
			mov_reg_mem(vm, byte);
		}
		else if ((instr >> 1) == MOV_IMM_REG_MEM)
		{
			mov_imm_reg_mem(vm, byte);
		}
		else if ((instr >> 1) == MOV_MEM_ACC)
		{
			mov_mem_acc(vm, byte);
		}
		else if ((instr >> 1) == MOV_ACC_MEM)
		{
			mov_acc_mem(vm, byte);
		}
		else if ((instr >> 4) == MOV_IMM_REG)
		{
			mov_imm_reg(vm, byte);
		}
		else if ((instr >> 2) == ADD_REG_MEM)
		{
			//add_reg_mem(vm, byte);
		}
		else if ((instr >> 1) == ADD_IMM_ACC)
		{
			//add_imm_acc(vm, byte);
		}
		else if ((instr >> 2) == SUB_REG_MEM)
		{
			sub_reg_mem(vm, byte);
		}
		else if ((instr >> 1) == SUB_IMM_ACC)
		{
			sub_imm_acc(vm, byte);
		}
		else if ((instr >> 2) == CMP_REG_MEM)
		{
			cmp_reg_mem(vm, byte);
		}
		else if ((instr >> 1) == CMP_IMM_ACC)
		{
			cmp_imm_acc(vm, byte);
		}
		else if ((instr >> 2) == ARM_IMM_REG_MEM)
		{
			arm_imm_reg_mem(vm, byte);
		}
		else
		{
			//TODO: Handle error (invalid instructions)
		}
	}
}

void rcvm_print_regs(rcvm_t *vm)
{
	const char *regs[] = {
		"REG_A", "REG_C", "REG_D", "REG_B",
		"REG_SP", "REG_BP", "REG_SI", "REG_DI",
		"REG_IP", "REG_FLAGS"
	};
	
	for (usize i = 0; i < REG_COUNT; i++)
	{
		printf("; %-10s: 0x%04X (%d)\n", regs[i], vm->registers[i], vm->registers[i]);
	}	
}

void rcvm_print_memory(rcvm_t *vm)
{
	usize count = 0;
	uint8 *byte = vm->memory;
	uint16 address = 0;

	printf("; 0x%04X: ", address);
	while (byte < vm->memory + MEMORY_MAX)
	{
		if (count == 32)
		{
			printf("\n; 0x%04X: ", address += 32);
			count = 0;
		}
		printf("%02X%02X ", *byte, *(byte + 1));
		byte += 2;
		count++;
	}
	printf("\n");
}

void rcvm_print_memory_from_to(rcvm_t *vm, usize lower, usize upper)
{
	//TODO: Handle error (upper bound)
	usize count = 0;
	uint8 *byte = &vm->memory[lower];
	uint16 address = lower;

	printf("; 0x%04X: ", address);
	while (byte < vm->memory + upper)
	{
		if (count == 32)
		{
			printf("\n; 0x%04X: ", address += 32);
			count = 0;
		}
		printf("%02X%02X ", *byte, *(byte + 1));
		byte += 2;
		count++;
	}
	printf("\n");
}

rcvm_t *rcvm_init(void)
{
	rcvm_t *new_vm = (rcvm_t *)calloc(1, sizeof(rcvm_t));
	if (new_vm)
	{
		return (new_vm);	
	} 

	//TODO: Handle error	
	return (NULL);
}

void rcvm_destroy(rcvm_t *vm)
{
	//TODO: Add secure freeing
	if (vm)
		free(vm);
}
