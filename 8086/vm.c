#include "vm.h"
#include <stdbool.h>
#include <string.h>

#define MEMORY_MAX ((1 << 16) * 2)
#define MEMORY_OFFSET 0x159F
#define internal_var static
#define internal_func static

internal_var uint8 vm_memory[MEMORY_MAX];
internal_var uint16 vm_registers[REG_COUNT];

internal_func inline const char *get_reg_name(registers reg, bool is_word)
{
	const char *reg_name_byte[] = { "al", "cl", "dl", "bl", "ah", "ch", "dl", "bl" };
	const char *reg_name_word[] = { "ax", "cx", "dx", "bl", "sp", "bp", "si", "di" };

	return ((is_word) ? reg_name_word[reg] : reg_name_byte[reg]);	
}

internal_func inline const char *get_disp_name(uint8 disp)
{
	const char *disp_name[] = { 
		"bx + si", "bx + di", "bp + si", "bp + di", "si", "di", "bp", "bx"
	};
	return (disp_name[disp]);
}

internal_func inline uint8 mem_read(uint16 address)
{
	return (vm_memory[address + MEMORY_OFFSET]);
}

internal_func inline void mem_write(uint16 address, uint8 value)
{
	vm_memory[address + MEMORY_OFFSET] = value;	
}

internal_func inline uint16 reg_read(registers reg)
{
	return (vm_registers[reg]);
}

internal_func inline void reg_write(registers reg, uint16 value)
{
	vm_registers[reg] = value;
}

internal_func void load_program_data(usize size, uint8 *data)
{
	memcpy(vm_memory, data, size);
}

internal_func void load_program_from_file(usize *size, const char *path)
{
	uint8 *data = file_read_binary(path, size);
   	memcpy(vm_memory, data, *size);	
}

internal_func void execute_program(usize program_size)
{
	for (uint8 *byte = vm_memory;
		byte != &vm_memory[program_size];
		byte++)
	{
		uint8 instr = *byte;
		if ((instr >> 2) == 0b100010)
		{
			printf("mov\n");	
		}
	}
}

internal_func void print_regs(void)
{
	const char *regs[] = {
		"REG_A", "REG_C", "REG_D", "REG_B",
		"REG_SP", "REG_BP", "REG_SI", "REG_DI",
		"REG_IP", "REG_FLAGS"
	};
	
	for (usize i = 0; i < REG_COUNT; i++)
	{
		printf("%-10s: 0x%04X (%d)\n", regs[i], vm_registers[i], vm_registers[i]);
	}	
}

internal_func void print_memory(void)
{
	usize count = 0;
	uint8 *byte = vm_memory;
	uint16 address = 0;

	printf("0x%04X: ", address);
	while (byte < vm_memory + MEMORY_MAX)
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

internal_func void print_memory_from_to(usize lower, usize upper)
{
	usize count = 0;
	uint8 *byte = &vm_memory[lower];
	uint16 address = lower;

	printf("0x%04X: ", address);
	while (byte < vm_memory + upper)
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

#undef internal_var
#undef internal_func

void vm_init_ptr(vm_t *vm)
{
	vm->mem_read = mem_read;
	vm->mem_write = mem_write;
	vm->reg_read = reg_read;
	vm->reg_write = reg_write;
	vm->load_program_data = load_program_data;
	vm->load_program_from_file = load_program_from_file;	
	vm->execute_program = execute_program;
	vm->print_memory = print_memory;
	vm->print_regs = print_regs;
	vm->print_memory_from_to = print_memory_from_to;
}

vm_t vm_init(void)
{
	vm_t new_vm = {0};

	new_vm.mem_read = mem_read;
	new_vm.mem_write = mem_write;
	new_vm.reg_read = reg_read;
	new_vm.reg_write = reg_write;
	new_vm.load_program_data = load_program_data;
	new_vm.load_program_from_file = load_program_from_file;
	new_vm.execute_program = execute_program;
	new_vm.print_memory = print_memory;
	new_vm.print_regs = print_regs;
	new_vm.print_memory_from_to = print_memory_from_to;

	return (new_vm);
}
