#include "instructions.h"

static inline const char *rcvm_get_reg_name(rcvm_registers_t reg, bool is_word)
{
	//TODO: Handle error
	const char *reg_name_byte[] = { "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh" };
	const char *reg_name_word[] = { "ax", "cx", "dx", "bx", "sp", "bp", "si", "di" };

	return ((is_word) ? reg_name_word[reg] : reg_name_byte[reg]);	
}

static inline const char *rcvm_get_disp_name(uint8 disp)
{
	//TODO: Handle error
	const char *disp_name[] = { 
		"bx + si", "bx + di", "bp + si", "bp + di", "si", "di", "bp", "bx"
	};
	return (disp_name[disp]);
}

static inline uint16 rcvm_get_disp(rcvm_t *vm, uint8 disp)
{
	switch (disp)
	{
		case 0b000:
		{
			return (rcvm_reg_read(vm, REG_B) + rcvm_reg_read(vm, REG_SI));
		} break;
		case 0b001:
		{
			return (rcvm_reg_read(vm, REG_B) + rcvm_reg_read(vm, REG_DI));
		} break;
		case 0b010:
		{
			return (rcvm_reg_read(vm, REG_BP) + rcvm_reg_read(vm, REG_SI));
		} break;
		case 0b011:
		{
			return (rcvm_reg_read(vm, REG_BP) + rcvm_reg_read(vm, REG_DI));
		} break;
		case 0b100:
		{
			return (rcvm_reg_read(vm, REG_SI));
		} break;
		case 0b101:
		{
			return (rcvm_reg_read(vm, REG_DI));
		} break;
		case 0b110:
		{
			return (rcvm_reg_read(vm, REG_BP));
		} break;
		case 0b111:
		{
			return (rcvm_reg_read(vm, REG_B));
		} break;
	
		default:
		//TODO: Handle error
			break;
	}
}

void mov_reg_mem(rcvm_t *vm, uint8 *instr_byte)
{
	const bool D_FIELD = (*instr_byte >> 1) & 1;
	const bool W_FIELD = (*instr_byte     ) & 1;
	const uint8 MOD_FIELD = (*(instr_byte + 1) >> 6) & 0b11;
	const uint8 REG_FIELD = (*(instr_byte + 1) >> 3) & 0b111;
	const uint8 RM_FIELD = (*(instr_byte + 1)) & 0b111;

	switch (MOD_FIELD)
	{
		case 0b00:
		{
			if (D_FIELD)
			{
				if (RM_FIELD == 0b110)
				{
					const uint16 disp = *(instr_byte + 2) | (*(instr_byte + 3) << 8);

					printf("mov %s, [%d]\n", rcvm_get_reg_name(REG_FIELD, W_FIELD),
							disp);
					rcvm_reg_write(vm, REG_FIELD, rcvm_mem_read(vm, disp));
				}
				else
				{
					printf("mov %s, [%s]\n", rcvm_get_reg_name(REG_FIELD, W_FIELD),
							rcvm_get_disp_name(RM_FIELD));
					rcvm_reg_write(vm, REG_FIELD, rcvm_mem_read(vm, rcvm_get_disp(vm, RM_FIELD)));
				}
			}
			else
			{
				if (RM_FIELD == 0b110)
				{
					const uint16 disp = *(instr_byte + 2) | (*(instr_byte + 3) << 8);
					printf("mov [%d], %s\n",
							disp, rcvm_get_reg_name(REG_FIELD, W_FIELD));
					rcvm_mem_write(vm, disp, rcvm_reg_read(vm, REG_FIELD));
				}
				else
				{
					printf("mov [%s], %s\n", rcvm_get_disp_name(RM_FIELD),
							rcvm_get_reg_name(REG_FIELD, W_FIELD));
					rcvm_mem_write(vm, rcvm_get_disp(vm, RM_FIELD), rcvm_reg_read(vm, REG_FIELD));
				}
			}

			if (RM_FIELD == 0b110)
			{
				rcvm_reg_write(vm, REG_IP, rcvm_reg_read(vm, REG_IP) + 4);
			}
			else
			{
				rcvm_reg_write(vm, REG_IP, rcvm_reg_read(vm, REG_IP) + 2);
			}
		} break;

		case 0b01:
		{
			const uint8 disp = *(instr_byte + 2);
			if (D_FIELD)
			{
				printf("mov %s, [%s %c %d]\n", rcvm_get_reg_name(REG_FIELD, W_FIELD),
						rcvm_get_disp_name(RM_FIELD), (disp < 0) ? '-' : '+', abs(disp));
				rcvm_reg_write(vm, REG_FIELD, rcvm_mem_read(vm, rcvm_get_disp(vm, RM_FIELD) + disp));
			}
			else
			{
				printf("mov [%s %c %d], %s\n", rcvm_get_disp_name(RM_FIELD), (disp < 0) ? '-' : '+', abs(disp),
						rcvm_get_reg_name(REG_FIELD, W_FIELD));
				rcvm_mem_write(vm, rcvm_get_disp(vm, REG_FIELD) + disp, rcvm_reg_read(vm, REG_FIELD));
			}
			rcvm_reg_write(vm, REG_IP, rcvm_reg_read(vm, REG_IP) + 3);
		} break;

		case 0b10:
		{
			const uint16 disp = *(instr_byte + 2) | (*(instr_byte + 3) << 8);
			if (D_FIELD)
			{
				printf("mov %s, [%s %c %d]\n", rcvm_get_reg_name(REG_FIELD, W_FIELD),
							rcvm_get_disp_name(RM_FIELD), (disp < 0) ? '-' : '+', abs(disp));
				rcvm_reg_write(vm, REG_FIELD, rcvm_mem_read(vm, rcvm_get_disp(vm, RM_FIELD) + disp));
			}
			else
			{
				
				printf("mov [%s %c %d], %s\n", rcvm_get_disp_name(RM_FIELD), (disp < 0) ? '-' : '+', abs(disp),
						rcvm_get_reg_name(REG_FIELD, W_FIELD));
				rcvm_mem_write(vm, rcvm_get_disp(vm, REG_FIELD) + disp, rcvm_reg_read(vm, REG_FIELD));
			}
			rcvm_reg_write(vm, REG_IP, rcvm_reg_read(vm, REG_IP) + 4);
		} break;

		case 0b11:
		{
			if (D_FIELD)
			{
				printf("mov %s, %s\n", rcvm_get_reg_name(REG_FIELD, W_FIELD),
						rcvm_get_reg_name(RM_FIELD, W_FIELD));
				rcvm_reg_write(vm, REG_FIELD, rcvm_reg_read(vm, RM_FIELD));
			}
			else
			{
				printf("mov %s, %s\n", rcvm_get_reg_name(RM_FIELD, W_FIELD),
						rcvm_get_reg_name(REG_FIELD, W_FIELD));
				rcvm_reg_write(vm, RM_FIELD, rcvm_reg_read(vm, REG_FIELD));
			}
			rcvm_reg_write(vm, REG_IP, rcvm_reg_read(vm, REG_IP) + 2);
		} break;
	}
}

void mov_imm_reg_mem(rcvm_t *vm, uint8 *instr_byte)
{
	const bool W_FIELD = *instr_byte & 1;
	const uint8 MOD_FIELD = (*(instr_byte + 1) >> 6) & 0b11;
	const uint8 RM_FIELD = (*(instr_byte + 1)) & 0b111;

	switch (MOD_FIELD)
	{
		case 0b00:
		{
			if (RM_FIELD == 0b110)
			{
				const uint16 disp = *(instr_byte + 2) | (*(instr_byte + 3) << 8);
				if (W_FIELD)
				{
					const uint16 data = *(instr_byte + 4) | (*(instr_byte + 5) << 8);
					printf("mov word [%d], %d\n", disp, data);
					rcvm_reg_write(vm, REG_IP, rcvm_reg_read(vm, REG_IP) + 6);
				}
				else
				{
					const uint8 data = *(instr_byte + 4);
					printf("mov byte[%d], %d\n", disp, data);
					rcvm_reg_write(vm, REG_IP, rcvm_reg_read(vm, REG_IP) + 5);
				}
			}
			else
			{
				if (W_FIELD)
				{
					const uint16 data = *(instr_byte + 2) | (*(instr_byte + 3) << 8);
					printf("mov word [%s], %d\n", rcvm_get_disp_name(RM_FIELD), data);
					rcvm_reg_write(vm, REG_IP, rcvm_reg_read(vm, REG_IP) + 4);
				}
				else
				{
					const uint8 data = *(instr_byte + 2);
					printf("mov byte [%s], %d\n", rcvm_get_disp_name(RM_FIELD), data);
					rcvm_reg_write(vm, REG_IP, rcvm_reg_read(vm, REG_IP) + 3);
				}
			}
		} break;
		case 0b01:
		{
			if (W_FIELD)
			{
				const uint8 disp = *(instr_byte + 2);
				const uint16 data = *(instr_byte + 3) | (*(instr_byte + 4) << 8);
				printf("mov word [%s %c %d], %d\n", rcvm_get_disp_name(RM_FIELD), (disp < 0) ? '-' : '+', disp, data);
				rcvm_reg_write(vm, REG_IP, rcvm_reg_read(vm, REG_IP) + 5);
			}
			else
			{
				const uint8 disp = *(instr_byte + 2);
				const uint8 data = *(instr_byte + 3);
				printf("mov byte [%s %c %d], %d\n", rcvm_get_disp_name(RM_FIELD), (disp < 0) ? '-' : '+', disp, data);
				rcvm_reg_write(vm, REG_IP, rcvm_reg_read(vm, REG_IP) + 4);
			}
		} break;
		case 0b10:
		{
			if (W_FIELD)
			{
				const uint16 disp = *(instr_byte + 2) | (*(instr_byte + 3) << 8);
				const uint16 data = *(instr_byte + 4) | (*(instr_byte + 5) << 8);
				printf("mov word [%s %c %d], %d\n", rcvm_get_disp_name(RM_FIELD), (disp < 0) ? '-' : '+', disp, data);
				rcvm_reg_write(vm, REG_IP, rcvm_reg_read(vm, REG_IP) + 6);
			}
			else
			{
				const uint16 disp = *(instr_byte + 2) | (*(instr_byte + 3) << 8);
				const uint8 data = *(instr_byte + 4);
				printf("mov byte [%s %c %d], %d\n", rcvm_get_disp_name(RM_FIELD), (disp < 0) ? '-' : '+', disp, data);
					rcvm_reg_write(vm, REG_IP, rcvm_reg_read(vm, REG_IP) + 5);
			}
		} break;
		case 0b11:
		{
			if (W_FIELD)
			{
				const uint16 data = *(instr_byte + 2) | (*(instr_byte + 3) << 8);
				printf("mov %s, %d\n", rcvm_get_reg_name(RM_FIELD, W_FIELD), data);
				rcvm_reg_write(vm, RM_FIELD, data);
				rcvm_reg_write(vm, REG_IP, rcvm_reg_read(vm, REG_IP) + 4);
			}
			else
			{
				const uint8 data = *(instr_byte + 2);
				printf("mov %s, %d\n", rcvm_get_reg_name(RM_FIELD, W_FIELD), data);
				rcvm_reg_write(vm, RM_FIELD, data);
				rcvm_reg_write(vm, REG_IP, rcvm_reg_read(vm, REG_IP) + 3);
			}
		} break;
	}
}

void mov_imm_reg(rcvm_t *vm, uint8 *instr_byte)
{
	const bool W_FIELD = (*instr_byte >> 3) & 1;
	const uint8 REG_FIELD = (*instr_byte) & 0b111;

	if (W_FIELD)
	{
		const uint16 data = (*(instr_byte + 1)) | (*(instr_byte + 2) << 8);
		printf("mov %s, %d\n", rcvm_get_reg_name(REG_FIELD, W_FIELD), data);
		rcvm_reg_write(vm, REG_FIELD, data);
		rcvm_reg_write(vm, REG_IP, rcvm_reg_read(vm, REG_IP) + 3);
	} 
	else
	{
		const uint8 data = (*(instr_byte + 1));
		printf("mov %s, %d\n", rcvm_get_reg_name(REG_FIELD, W_FIELD), data);
		rcvm_reg_write(vm, REG_FIELD, data);
		rcvm_reg_write(vm, REG_IP, rcvm_reg_read(vm, REG_IP) + 2);
	}
}

void mov_mem_acc(rcvm_t *vm, uint8 *instr_byte)
{
	const bool W_FIELD = (*instr_byte) & 1;
	const uint16 address = (*(instr_byte + 1)) | (*(instr_byte + 2) << 8);

	if (W_FIELD)
	{
		printf("mov ax, [%d]\n", address);
	}
	else
	{
		printf("mov al, [%d]\n", address);
	}
	rcvm_reg_write(vm, REG_A, rcvm_mem_read(vm, address));
	rcvm_reg_write(vm, REG_IP, rcvm_reg_read(vm, REG_IP) + 3);
}

void mov_acc_mem(rcvm_t *vm, uint8 *instr_byte)
{
	const bool W_FIELD = (*instr_byte) & 1;
	const uint16 address = (*(instr_byte + 1)) | (*(instr_byte + 2) << 8);

	if (W_FIELD)
	{
		printf("mov [%d], ax\n", address);
	}
	else
	{
		printf("mov [%d], al\n", address);
	}
	rcvm_mem_write(vm, address, rcvm_reg_read(vm, REG_A));
	rcvm_reg_write(vm, REG_IP, rcvm_reg_read(vm, REG_IP) + 3);
}

// Adds (e.g. add ax, bx)
void add_reg_mem(rcvm_t *vm, uint8 *instr_byte)
{

}

void add_imm_acc(rcvm_t *vm, uint8 *instr_byte)
{

}

// Subs (e.g. sub ax, bx)
void sub_reg_mem(rcvm_t *vm, uint8 *instr_byte)
{

}

void sub_imm_acc(rcvm_t *vm, uint8 *instr_byte)
{

}

// Cmps(e.g. cmp ax, bx)
void cmp_reg_mem(rcvm_t *vm, uint8 *instr_byte)
{

}

void cmp_imm_acc(rcvm_t *vm, uint8 *instr_byte)
{

}

// All arithmetic immediate operations
void arm_imm_reg_mem(rcvm_t *vm, uint8 *instr_byte)
{

}
