#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "vm.h"

static uint16_t registers[REG_COUNT];
static uint8_t memory[MEM_SIZE];

static uint8_t *read_binary_file(const char *path, size_t *size)
{
	FILE *file = fopen(path, "rb");
	ASSERT(file, "Failed to open binary file!\n");
	fseek(file, 0, SEEK_END);
	size_t len = ftell(file);
    fseek(file, 0, SEEK_SET);

	uint8_t *data = (uint8_t *)malloc(len);
	ASSERT(data, "Allocation failed!\n");
	fread(data, len, 1, file);
	if (size)
		*size = len;
	fclose(file);
	return (data);	
}

static void write_binary_file_to_memory(const char *path, size_t *size)
{
	FILE *file = fopen(path, "rb");
	ASSERT(file, "Failed to open binary file!\n");
	fseek(file, 0, SEEK_END);
	size_t len = ftell(file);
    fseek(file, 0, SEEK_SET);

	fread(memory, len, 1, file);
	if (size)
		*size = len;
	fclose(file);
}

static inline uint8_t mem_read(uint8_t address)
{
	return (memory[address + MEM_OFFSET]);
}

static inline void mem_write(uint8_t address, uint8_t value)
{
	memory[address + MEM_OFFSET] = value;
}

static void print_regs(void)
{
	const char *reg_names[] = { "REG_A", "REG_C", "REG_D", "REG_B", "REG_SP", "REG_BP", "REG_SI", "REG_DI", "REG_IP", "REG_FLAGS" };
	for (int i = 0; i < REG_COUNT; i++)
	{
		printf("; %s:   0x%x (%d)\n", reg_names[i], registers[i], registers[i]);
	}
}

static const char *get_reg_name(uint8_t reg, bool is_word)
{
	const char *reg_names_byte[] = { "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh" };
	const char *reg_names_word[] = { "ax", "cx", "dx", "bx", "sp", "bp", "si", "di" };

	return (is_word) ? reg_names_word[reg] : reg_names_byte[reg];
}

static const char *get_disp_name(uint8_t reg)
{
	const char *disp_names[] = { "bx + si", "bx + di", "bp + si", "bp + di", "si", "di", "bp", "bx" };
	return (disp_names[reg]);
}

static uint16_t get_disp(uint8_t rm_field)
{
	switch (rm_field)
	{
		case 0b0:
		{
			return (registers[REG_B] + registers[REG_SI]);
		} break;
		case 0b1:
		{
			return (registers[REG_B] + registers[REG_DI]);
		} break;
		case 0b10:
		{
			return (registers[REG_BP] + registers[REG_SI]);
		} break;
		case 0b11:
		{
			return (registers[REG_BP] + registers[REG_DI]);
		} break;
		case 0b100:
		{
			return (registers[REG_SI]);
		} break;
		case 0b101:
		{
			return (registers[REG_DI]);
		} break;
		case 0b110:
		{
			return (registers[REG_BP]);
		} break;
		case 0b111:
		{
			return (registers[REG_B]);
		} break;
		default:
		{
			ASSERT(false, "Not valid RM_FIELD!\n");
		} break;
	}
	return (0);
}

static void set_flag(reg r)
{
	if (registers[r] == 0) 
	{
		registers[REG_FLAGS] = (registers[REG_FLAGS] | FLAG_ZERO);
	}
	else if (registers[r] != 0 && (registers[REG_FLAGS] & FLAG_ZERO))
	{
		registers[REG_FLAGS] = (registers[REG_FLAGS] ^ FLAG_ZERO);
	}

	if ((registers[r] >> 7)) 
	{
		registers[REG_FLAGS] = (registers[REG_FLAGS] | FLAG_SIGN);
	}
	else if (!(registers[r] >> 7) && (registers[REG_FLAGS] & FLAG_SIGN))
	{
		registers[REG_FLAGS] = (registers[REG_FLAGS] ^ FLAG_SIGN);
	} 
}

static void mov_reg_mem(uint8_t *byte)
{
	const bool D_FIELD = (*byte >> 1) & 1;
	const bool W_FIELD = (*byte 	  ) & 1;
	const uint8_t MOD_FIELD = (*(byte + 1) >> 6) & 0b11;
	const uint8_t REG_FIELD = (*(byte + 1) >> 3) & 0b111;
	const uint8_t RM_FIELD  = (*(byte + 1)     ) & 0b111;

	switch (MOD_FIELD)
	{
		case 0b00:
		{
			if (D_FIELD)
			{
				printf("mov %s, [%s] ; /=%p=/\n", get_reg_name(REG_FIELD, W_FIELD), get_disp_name(RM_FIELD), byte);
				registers[REG_FIELD] = mem_read(get_disp(RM_FIELD)) | (mem_read(get_disp(RM_FIELD) + 1) << 8);	
			}
			else
			{
					
				printf("mov %s [%s], %s ; /=%p=/\n", (W_FIELD) ? "word" : "byte",  get_disp_name(RM_FIELD), get_reg_name(REG_FIELD, W_FIELD), byte);
				mem_write(get_disp(RM_FIELD), registers[REG_FIELD]);
			}
			registers[REG_IP] += 2;
		} break;
		case 0b01:
		{
			const uint8_t disp = (*(byte + 2)); 
			if (D_FIELD)
			{
				printf("mov %s, [%s + %d] ; /=%p=/\n", get_reg_name(REG_FIELD, W_FIELD), get_disp_name(RM_FIELD), disp, byte);
				registers[REG_FIELD] = mem_read(get_disp(RM_FIELD) + disp) | (mem_read(get_disp(RM_FIELD) + disp + 1) << 8);	
			}
			else
			{
					
				printf("mov %s [%s + %d], %s ; /=%p=/\n", (W_FIELD) ? "word" : "byte",  get_disp_name(RM_FIELD), disp,  get_reg_name(REG_FIELD, W_FIELD), byte);
				mem_write(get_disp(RM_FIELD) + disp, registers[REG_FIELD]);
			}
			registers[REG_IP] += 3;
		} break;
		case 0b10:
		{
			const uint8_t disp_low = (*(byte + 2));
		    const uint16_t disp_high = (*(byte + 3) << 8);
			const uint16_t disp = disp_low | disp_high;
			if (D_FIELD)
			{
				printf("mov %s, [%s + %d] ; /=%p=/\n", get_reg_name(REG_FIELD, W_FIELD), get_disp_name(RM_FIELD), disp, byte);	
				registers[REG_FIELD] = mem_read(get_disp(RM_FIELD) + disp) | (mem_read(get_disp(RM_FIELD) + disp + 1) << 8);	
				
			}
			else
			{
					
				printf("mov %s [%s + %d], %s ; /=%p=/\n", (W_FIELD) ? "word" : "byte",  get_disp_name(RM_FIELD), disp,  get_reg_name(REG_FIELD, W_FIELD), byte);
				mem_write(get_disp(RM_FIELD) + disp, registers[REG_FIELD]);
			}
			registers[REG_IP] += 4;
		} break;
		case 0b11:
		{
			if (D_FIELD)
			{
				printf("mov %s, %s ; /=%p=/\n", get_reg_name(REG_FIELD, W_FIELD), get_reg_name(RM_FIELD, W_FIELD), byte);
				registers[REG_FIELD] = registers[RM_FIELD];	
			}
			else
			{
					
				printf("mov %s, %s ; /=%p=/\n", get_reg_name(RM_FIELD, W_FIELD), get_reg_name(REG_FIELD, W_FIELD), byte);
				registers[RM_FIELD] = registers[REG_FIELD];	
			}

			registers[REG_IP] += 2;
		} break;
	}
}

static void mov_imm_reg_mem(uint8_t *byte)
{
	const bool W_FIELD = (*byte 	  ) & 1;
	const uint8_t MOD_FIELD = (*(byte + 1) >> 6) & 0b11;
	const uint8_t RM_FIELD  = (*(byte + 1)     ) & 0b111;
	
	switch (MOD_FIELD)
	{
		case 0b00:
		{
			if (W_FIELD)
			{
				const uint8_t data_low = *(byte + 2);
				const uint8_t data_high = *(byte + 3);
				const uint16_t data = data_low | (data_high << 8);

				printf("mov %s [%s], %d ; /=%p=/\n", (W_FIELD) ? "word" : "byte",  get_disp_name(RM_FIELD), data, byte);
				mem_write(get_disp(RM_FIELD), data_low);
				mem_write(get_disp(RM_FIELD) + 1, data_high);
				registers[REG_IP] += 4;
			}
			else
			{
				const uint8_t data  = *(byte + 2);		
				printf("mov %s [%s], %d ; /=%p=/\n", (W_FIELD) ? "word" : "byte",  get_disp_name(RM_FIELD), data, byte);
				mem_write(get_disp(RM_FIELD), data);
				registers[REG_IP] += 3;
			}
			 
		} break;
		case 0b01:
		{
			const uint8_t disp = (*(byte + 2));
			if (W_FIELD)
			{
				const uint8_t data_low = *(byte + 3);
				const uint8_t data_high = *(byte + 4);
				const uint16_t data = data_low | (data_high << 8);
				printf("mov %s [%s + %d], %d ; /=%p=/\n", (W_FIELD) ? "word" : "byte",  get_disp_name(RM_FIELD), disp, data, byte);

				mem_write(get_disp(RM_FIELD) + disp, data_low);
				mem_write(get_disp(RM_FIELD) + disp + 1, data_high); 
				registers[REG_IP] += 5;
			}
			else
			{
				const uint8_t data = *(byte + 3);
				printf("mov %s [%s + %d], %d ; /=%p=/\n", (W_FIELD) ? "word" : "byte",  get_disp_name(RM_FIELD), disp, data, byte);
				mem_write(get_disp(RM_FIELD) + disp, data);
				registers[REG_IP] += 4;
			}
		} break;
		case 0b10:
		{
			uint8_t disp_low = (*(byte + 2));
			uint16_t disp_high = (*(byte + 3));
			uint16_t disp = disp_low | disp_high;
			if (W_FIELD)
			{
				const uint8_t data_low = *(byte + 4);
				const uint8_t data_high = *(byte + 5);
				const uint16_t data = data_low | data_high << 8;
				printf("mov %s [%s + %d], %d ; /=%p=/\n", (W_FIELD) ? "word" : "byte",  get_disp_name(RM_FIELD), disp, data, byte);

				mem_write(get_disp(RM_FIELD) + disp, data_low);
				mem_write(get_disp(RM_FIELD) + disp + 1, data_high);
				registers[REG_IP] += 6;
			}
			else
			{
				const uint8_t data = *(byte + 4);
				printf("mov %s [%s + %d], %d ; /=%p=/\n", (W_FIELD) ? "word" : "byte",  get_disp_name(RM_FIELD), disp, data, byte);
				mem_write(get_disp(RM_FIELD) + disp, data);
				registers[REG_IP] += 5;
			}
		} break;
		case 0b11:
		{
			if (W_FIELD)
			{
				const uint8_t data_low = *(byte + 2);
				const uint8_t data_high = *(byte + 3);
				const uint16_t data = data_low | (data_high << 8);

				printf("mov %s, %d ; /=%p=/\n", get_reg_name(RM_FIELD, W_FIELD), data, byte);
				registers[RM_FIELD] = data;
				registers[REG_IP] += 4;
			}
			else
			{
				const uint8_t data  = *(byte + 2);		
				printf("mov %s, %d ; /=%p=/\n", get_reg_name(RM_FIELD, W_FIELD), data, byte);
				registers[RM_FIELD] = data;
				registers[REG_IP] += 3;
			}
		} break;
	}
}

static void mov_imm_reg(uint8_t *byte)
{
	const bool W_FIELD = (*byte >> 3) & 1;
	const uint8_t REG_FIELD = (*byte) & 0b111;

	if (W_FIELD)
	{
		const uint8_t data_low = *(byte + 1);
		const uint16_t data_high = *(byte + 2) << 8;
		const uint16_t data = data_low | data_high;
		printf("mov %s, %d ; /=%p=/\n", get_reg_name(REG_FIELD, W_FIELD), data, byte);
		registers[REG_FIELD] = data;
		registers[REG_IP] += 3;
	}
	else
	{
		const uint8_t data = *(byte + 1);
		printf("mov %s, %d ; /=%p=/\n", get_reg_name(REG_FIELD, W_FIELD), data, byte);
		registers[REG_FIELD] = data;	
		registers[REG_IP] += 2;
	}
}

static void add_reg_mem(uint8_t *byte)
{
	const bool D_FIELD = (*byte >> 1) & 1;
	const bool W_FIELD = (*byte 	  ) & 1;
	const uint8_t MOD_FIELD = (*(byte + 1) >> 6) & 0b11;
	const uint8_t REG_FIELD = (*(byte + 1) >> 3) & 0b111;
	const uint8_t RM_FIELD  = (*(byte + 1)     ) & 0b111;

	switch (MOD_FIELD)
	{
		case 0b00:
		{
			if (D_FIELD)
			{
				printf("add %s, [%s] ; /=%p=/\n", get_reg_name(REG_FIELD, W_FIELD), get_disp_name(RM_FIELD), byte);
				registers[REG_FIELD] += mem_read(get_disp(RM_FIELD)) | (mem_read(get_disp(RM_FIELD) + 1) << 8);	
				set_flag(REG_FIELD);
			}
			else
			{
					
				printf("add %s [%s], %s ; /=%p=/\n", (W_FIELD) ? "word" : "byte",  get_disp_name(RM_FIELD), get_reg_name(REG_FIELD, W_FIELD), byte);
				mem_write(get_disp(RM_FIELD), registers[REG_FIELD] + mem_read(get_disp(RM_FIELD)));
			}
			registers[REG_IP] += 2;
		} break;
		case 0b01:
		{
			const uint8_t disp = (*(byte + 2)); 
			if (D_FIELD)
			{
				printf("add %s, [%s + %d] ; /=%p=/\n", get_reg_name(REG_FIELD, W_FIELD), get_disp_name(RM_FIELD), disp, byte);
				registers[REG_FIELD] += mem_read(get_disp(RM_FIELD) + disp) | (mem_read(get_disp(RM_FIELD) + disp + 1) << 8);
				set_flag(REG_FIELD);
			}
			else
			{
					
				printf("add %s [%s + %d], %s ; /=%p=/\n", (W_FIELD) ? "word" : "byte",  get_disp_name(RM_FIELD), disp,  get_reg_name(REG_FIELD, W_FIELD), byte);
				mem_write(get_disp(RM_FIELD) + disp, registers[REG_FIELD] + mem_read(get_disp(RM_FIELD) + disp));
			}
			registers[REG_IP] += 3;
		} break;
		case 0b10:
		{
			const uint8_t disp_low = (*(byte + 2));
		    const uint16_t disp_high = (*(byte + 3) << 8);
			const uint16_t disp = disp_low | disp_high;
			if (D_FIELD)
			{
				printf("add %s, [%s + %d] ; /=%p=/\n", get_reg_name(REG_FIELD, W_FIELD), get_disp_name(RM_FIELD), disp, byte);	
				registers[REG_FIELD] += mem_read(get_disp(RM_FIELD) + disp) | (mem_read(get_disp(RM_FIELD) + disp + 1) << 8);	
				set_flag(REG_FIELD);
			}
			else
			{
					
				printf("add %s [%s + %d], %s ; /=%p=/\n", (W_FIELD) ? "word" : "byte",  get_disp_name(RM_FIELD), disp,  get_reg_name(REG_FIELD, W_FIELD), byte);
				mem_write(get_disp(RM_FIELD) + disp, registers[REG_FIELD] + mem_read(get_disp(RM_FIELD) + disp));
			}
			registers[REG_IP] += 4;
		} break;
		case 0b11:
		{
			if (D_FIELD)
			{
				printf("add %s, %s ; /=%p=/\n", get_reg_name(REG_FIELD, W_FIELD), get_reg_name(RM_FIELD, W_FIELD), byte);
				registers[REG_FIELD] += registers[RM_FIELD];
				set_flag(REG_FIELD);
			}
			else
			{
					
				printf("add %s, %s ; /=%p=/\n", get_reg_name(RM_FIELD, W_FIELD), get_reg_name(REG_FIELD, W_FIELD), byte);
				registers[RM_FIELD] += registers[REG_FIELD];
				set_flag(RM_FIELD);
			}

			registers[REG_IP] += 2;
		} break;
	}
}

static void add_imm_reg_mem(uint8_t *byte)
{
	const bool W_FIELD = (*byte 	  ) & 1;
	const bool S_FIELD = (*byte >> 1) & 1;
	const uint8_t MOD_FIELD = (*(byte + 1) >> 6) & 0b11;
	const uint8_t RM_FIELD  = (*(byte + 1)     ) & 0b111;
	
	switch (MOD_FIELD)
	{
		case 0b00:
		{
			if (W_FIELD && !S_FIELD)
			{
				const uint8_t data_low = *(byte + 2);
				const uint8_t data_high = *(byte + 3);
				const uint16_t data = data_low | (data_high << 8);

				printf("add %s [%s], %d ; /=%p=/\n", (W_FIELD) ? "word" : "byte",  get_disp_name(RM_FIELD), data, byte);
				mem_write(get_disp(RM_FIELD), data_low);
				mem_write(get_disp(RM_FIELD) + 1, data_high);
				registers[REG_IP] += 4;
			}
			else
			{
				const uint8_t data  = *(byte + 2);		
				printf("add %s [%s], %d ; /=%p=/\n", (W_FIELD) ? "word" : "byte",  get_disp_name(RM_FIELD), data, byte);
				mem_write(get_disp(RM_FIELD), data + mem_read(get_disp(RM_FIELD)));
				registers[REG_IP] += 3;
			}
			 
		} break;
		case 0b01:
		{
			uint8_t disp = (*(byte + 2));
			if (W_FIELD && !S_FIELD)
			{
				const uint8_t data_low = *(byte + 3);
				const uint8_t data_high = *(byte + 4);
				const uint16_t data = data_low | (data_high << 8);
				printf("add %s [%s + %d], %d ; /=%p=/\n", (W_FIELD) ? "word" : "byte",  get_disp_name(RM_FIELD), disp, data, byte);

				mem_write(get_disp(RM_FIELD) + disp, data_low);
				mem_write(get_disp(RM_FIELD) + disp + 1, data_high); 
				registers[REG_IP] += 5;
			}
			else
			{
				const uint8_t data = *(byte + 3);
				printf("add %s [%s + %d], %d ; /=%p=/\n", (W_FIELD) ? "word" : "byte",  get_disp_name(RM_FIELD), disp, data, byte);
				mem_write(get_disp(RM_FIELD) + disp, data + mem_read(get_disp(RM_FIELD) + disp));
				registers[REG_IP] += 4;
			}
		} break;
		case 0b10:
		{
			const uint8_t disp_low = (*(byte + 2));
			const 	const uint8_t disp_high = (*(byte + 3));
			const 	const uint16_t disp = disp_low | (disp_high << 8);
			if (W_FIELD && !S_FIELD)
			{
				const uint8_t data_low = *(byte + 4);
				const uint8_t data_high = *(byte + 5);
				const uint16_t data = data_low | data_high << 8;
				printf("add %s [%s + %d], %d ; /=%p=/\n", (W_FIELD) ? "word" : "byte",  get_disp_name(RM_FIELD), disp, data, byte);

				mem_write(get_disp(RM_FIELD) + disp, data_low);
				mem_write(get_disp(RM_FIELD) + disp + 1, data_high);
				registers[REG_IP] += 6;
			}
			else
			{
				const uint8_t data = *(byte + 4);
				printf("add %s [%s + %d], %d ; /=%p=/\n", (W_FIELD) ? "word" : "byte",  get_disp_name(RM_FIELD), disp, data, byte);
				mem_write(get_disp(RM_FIELD) + disp, data + mem_read(get_disp(RM_FIELD) + disp));
				registers[REG_IP] += 5;
			}
		} break;
		case 0b11:
		{
			if (W_FIELD && !S_FIELD)
			{
				const uint8_t data_low = *(byte + 2);
				const uint8_t data_high = *(byte + 3);
				const uint16_t data = data_low | (data_high << 8);

				printf("add %s, %d ; /=%p=/\n", get_reg_name(RM_FIELD, W_FIELD), data, byte);
				registers[RM_FIELD] += data;
				registers[REG_IP] += 4;
			}
			else
			{
				const uint8_t data  = *(byte + 2);		
		 		printf("add %s, %d ; /=%p=/\n", get_reg_name(RM_FIELD, W_FIELD), data, byte);
				registers[RM_FIELD] += data;
				registers[REG_IP] += 3;
			}
		} break;
	}
}

static void add_imm_acc(uint8_t *byte)
{
	const bool W_FIELD = *byte & 1;
	if (W_FIELD)
	{
		const uint8_t data_low = *(byte + 1);
		const uint8_t data_high = *(byte + 2);
		const uint16_t data = data_low | (data_high << 8);

		printf("add ax, %d\n", data);
		registers[REG_IP] += 3;
	}
	else
	{
		const uint8_t data = *(byte + 1);
		printf("add al, %d\n", data);
		registers[REG_IP] += 2;	
	}
}

static void sub_reg_mem(uint8_t *byte)
{
	bool D_FIELD = (*byte >> 1) & 1;
	bool W_FIELD = (*byte 	  ) & 1;
	uint8_t MOD_FIELD = (*(byte + 1) >> 6) & 0b11;
	uint8_t REG_FIELD = (*(byte + 1) >> 3) & 0b111;
	uint8_t RM_FIELD  = (*(byte + 1)     ) & 0b111;

	switch (MOD_FIELD)
	{
		case 0b00:
		{
			if (D_FIELD)
			{
				printf("sub %s, [%s] ; /=%p=/\n", get_reg_name(REG_FIELD, W_FIELD), get_disp_name(RM_FIELD), byte);
				registers[REG_FIELD] -= mem_read(get_disp(RM_FIELD)) | (mem_read(get_disp(RM_FIELD) + 1) << 8);	
				set_flag(REG_FIELD);
			}
			else
			{
					
				printf("sub %s [%s], %s ; /=%p=/\n", (W_FIELD) ? "word" : "byte",  get_disp_name(RM_FIELD), get_reg_name(REG_FIELD, W_FIELD), byte);
				mem_write(get_disp(RM_FIELD), registers[REG_FIELD] - mem_read(get_disp(RM_FIELD)));
			}
			registers[REG_IP] += 2;
		} break;
		case 0b01:
		{
			uint8_t disp = (*(byte + 2)); 
			if (D_FIELD)
			{
				printf("sub %s, [%s + %d] ; /=%p=/\n", get_reg_name(REG_FIELD, W_FIELD), get_disp_name(RM_FIELD), disp, byte);
				registers[REG_FIELD] -= mem_read(get_disp(RM_FIELD) + disp) | (mem_read(get_disp(RM_FIELD) + disp + 1) << 8);	
				set_flag(REG_FIELD);
			}
			else
			{
					
				printf("sub %s [%s + %d], %s ; /=%p=/\n", (W_FIELD) ? "word" : "byte",  get_disp_name(RM_FIELD), disp,  get_reg_name(REG_FIELD, W_FIELD), byte);
				mem_write(get_disp(RM_FIELD) + disp, registers[REG_FIELD] - mem_read(get_disp(RM_FIELD) + disp));
			}
			registers[REG_IP] += 3;
		} break;
		case 0b10:
		{
			uint8_t disp_low = (*(byte + 2));
		    uint16_t disp_high = (*(byte + 3) << 8);
			uint16_t disp = disp_low | disp_high;
			if (D_FIELD)
			{
				printf("sub %s, [%s + %d] ; /=%p=/\n", get_reg_name(REG_FIELD, W_FIELD), get_disp_name(RM_FIELD), disp, byte);	
				registers[REG_FIELD] -= mem_read(get_disp(RM_FIELD) + disp) | (mem_read(get_disp(RM_FIELD) + disp + 1) << 8);	
				set_flag(REG_FIELD);
			}
			else
			{
					
				printf("sub %s [%s + %d], %s ; /=%p=/\n", (W_FIELD) ? "word" : "byte",  get_disp_name(RM_FIELD), disp,  get_reg_name(REG_FIELD, W_FIELD), byte);
				mem_write(get_disp(RM_FIELD) + disp, registers[REG_FIELD] - mem_read(get_disp(RM_FIELD) + disp));
			}
			registers[REG_IP] += 4;
		} break;
		case 0b11:
		{
			if (D_FIELD)
			{
				printf("sub %s, %s ; /=%p=/\n", get_reg_name(REG_FIELD, W_FIELD), get_reg_name(RM_FIELD, W_FIELD), byte);
				registers[REG_FIELD] -= registers[RM_FIELD];	
				set_flag(REG_FIELD);
			}
			else
			{
					
				printf("sub %s, %s ; /=%p=/\n", get_reg_name(RM_FIELD, W_FIELD), get_reg_name(REG_FIELD, W_FIELD), byte);
				registers[RM_FIELD] -= registers[REG_FIELD];	
				set_flag(RM_FIELD);
			}

			registers[REG_IP] += 2;
		} break;
	}
}

static void execute_program(size_t size)
{
	for (uint8_t *byte = memory;
		byte != &memory[size];
		byte = memory + registers[REG_IP])
	{
		uint8_t instr = *byte;	
		if ((instr >> 2) == 0b100010)
		{
			mov_reg_mem(byte);	
		}
		else if ((instr >> 4) == 0b1011)
		{
			mov_imm_reg(byte);	
		}
		else if ((instr >> 1) == 0b1100011)
		{
			mov_imm_reg_mem(byte);
		}
		else if ((instr >> 2) == 0b000000)
		{
			add_reg_mem(byte);
		}
		else if ((instr >> 2) == 0b001010)
		{
			sub_reg_mem(byte);
		}
		else if ((instr >> 2) == 0b100000)
		{
			add_imm_reg_mem(byte);
		}
		else if ((instr >> 1) == 0b0000010)
		{
			add_imm_acc(byte);
		}
		else
		{
			ASSERT(false, "Not implemented!\n");
		}
	}

	print_regs();
}


int main(int argc, char **argv)
{
	size_t size;
	/*if (argc < 2)
	{
		fprintf(stderr, "Usage: vm [filename]\n");
		exit(1);
	}*/
	write_binary_file_to_memory("test", &size);
	execute_program(size);
}
