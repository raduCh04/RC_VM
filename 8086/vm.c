#include "vm.h"
#include <stdbool.h>
#include <string.h>

#define MEMORY_MAX ((1 << 16) * 2)
#define MEMORY_OFFSET 0x159F

struct virtual_machine
{
	uint8 memory[MEMORY_MAX];
	uint16 registers[REG_COUNT];	
};

static inline const char *get_reg_name(registers_t reg, bool is_word)
{
	const char *reg_name_byte[] = { "al", "cl", "dl", "bl", "ah", "ch", "dl", "bl" };
	const char *reg_name_word[] = { "ax", "cx", "dx", "bl", "sp", "bp", "si", "di" };

	return ((is_word) ? reg_name_word[reg] : reg_name_byte[reg]);	
}

static inline const char *get_disp_name(uint8 disp)
{
	const char *disp_name[] = { 
		"bx + si", "bx + di", "bp + si", "bp + di", "si", "di", "bp", "bx"
	};
	return (disp_name[disp]);
}

inline uint8 mem_read(vm_t *vm, uint16 address)
{
	return (vm->memory[address + MEMORY_OFFSET]);
}

inline void mem_write(vm_t *vm, uint16 address, uint8 value)
{
	vm->memory[address + MEMORY_OFFSET] = value;	
}

inline uint16 reg_read(vm_t *vm, registers_t reg)
{
	return (vm->registers[reg]);
}

inline void reg_write(vm_t *vm, registers_t reg, uint16 value)
{
	vm->registers[reg] = value;
}

void load_program_data(vm_t *vm, usize size, uint8 *data)
{
	memcpy(vm->memory, data, size);
}

void load_program_from_file(vm_t *vm, usize *size, const char *path)
{
	uint8 *data = file_read_binary(path, size);
   	memcpy(vm->memory, data, *size);	
}

void execute_program(vm_t *vm, usize program_size)
{
	for (uint8 *byte = vm->memory;
		byte != &vm->memory[program_size];
		byte++)
	{
		uint8 instr = *byte;
		if ((instr >> 2) == 0b100010)
		{
			printf("mov\n");	
		}
	}
}

void print_regs(vm_t *vm)
{
	const char *regs[] = {
		"REG_A", "REG_C", "REG_D", "REG_B",
		"REG_SP", "REG_BP", "REG_SI", "REG_DI",
		"REG_IP", "REG_FLAGS"
	};
	
	for (usize i = 0; i < REG_COUNT; i++)
	{
		printf("%-10s: 0x%04X (%d)\n", regs[i], vm->registers[i], vm->registers[i]);
	}	
}

void print_memory(vm_t *vm)
{
	usize count = 0;
	uint8 *byte = vm->memory;
	uint16 address = 0;

	printf("0x%04X: ", address);
	while (byte < vm->memory + MEMORY_MAX)
	{
		if (count == 32)
		{
			printf("\n0x%04X: ", address += 32);
			count = 0;
		}
		printf("%02X%02X ", *byte, *(byte + 1));
		byte += 2;
		count++;
	}
	printf("\n");
}

void print_memory_from_to(vm_t *vm, usize lower, usize upper)
{
	usize count = 0;
	uint8 *byte = &vm->memory[lower];
	uint16 address = lower;

	printf("0x%04X: ", address);
	while (byte < vm->memory + upper)
	{
		if (count == 32)
		{
			printf("\n0x%04X: ", address += 32);
			count = 0;
		}
		printf("%02X%02X ", *byte, *(byte + 1));
		byte += 2;
		count++;
	}
	printf("\n");
}

vm_t *vm_init(void)
{
	vm_t *new_vm = (vm_t *)calloc(1, sizeof(vm_t));
	if (new_vm)
	{
		return (new_vm);	
	} 

	//TODO: Handle error	
	return (NULL);
}
