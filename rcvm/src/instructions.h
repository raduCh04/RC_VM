#pragma once
#include "rcvm.h"


typedef enum rcvm_instruction_opcode
{
	MOV_REG_MEM = 0b100010,
	MOV_IMM_REG_MEM = 0b1100011,
	MOV_IMM_REG = 0b1011,
	MOV_MEM_ACC = 0b1010000,
	MOV_ACC_MEM = 0b1010001,


} rcvm_instr_opc_t;

// Movs (e.g. mov ax, bx)
void mov_reg_mem(rcvm_t *vm, uint8 *instr_byte);
void mov_imm_reg_mem(rcvm_t *vm, uint8 *instr_byte);
void mov_imm_reg(rcvm_t *vm, uint8 *instr_byte);
void mov_mem_acc(rcvm_t *vm, uint8 *instr_byte);
void mov_acc_mem(rcvm_t *vm, uint8 *instr_byte);

// Adds (e.g. add ax, bx)
void add_reg_mem(rcvm_t *vm, uint8 *instr_byte);
void add_imm_acc(rcvm_t *vm, uint8 *instr_byte);

// Subs (e.g. sub ax, bx)
void sub_reg_mem(rcvm_t *vm, uint8 *instr_byte);
void sub_imm_acc(rcvm_t *vm, uint8 *instr_byte);

// Cmps(e.g. cmp ax, bx)
void cmp_reg_mem(rcvm_t *vm, uint8 *instr_byte);
void cmp_imm_acc(rcvm_t *vm, uint8 *instr_byte);

// All arithmetic immediate operations
void arm_imm_reg_mem(rcvm_t *vm, uint8 *instr_byte);

