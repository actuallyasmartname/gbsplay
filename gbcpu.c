/* $Id: gbcpu.c,v 1.2 2003/08/24 03:26:10 ranma Exp $
 *
 * gbsplay is a Gameboy sound player
 *
 * 2003 (C) by Tobias Diedrich <ranma@gmx.at>
 * Licensed under GNU GPL.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/param.h>
#include <unistd.h>

#include "gbcpu.h"

#if DEBUG == 1
static char regnames[12] = "BCDEHLFASPPC";
static char *regnamech16[6] = {
	"BC", "DE", "HL", "FA", "SP", "PC"
};
static char *conds[4] = {
	"NZ", "Z", "NC", "C"
};
#endif

struct opinfo;

typedef void (*ex_fn)(unsigned char op, struct opinfo *oi);

struct opinfo {
#if DEBUG == 1
	char *name;
#endif
	ex_fn fn;
};

struct regs regs;
int halted;
int interrupts;

static unsigned char none_get(unsigned short addr)
{
	return 0xff;
}

static void none_put(unsigned short addr, unsigned char val)
{
}

static get_fn getlookup[256] = {
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get
};

static put_fn putlookup[256] = {
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put
};

static inline unsigned char mem_get(unsigned short addr)
{
	unsigned char res = getlookup[addr >> 8](addr);
	return res;
}

static inline void mem_put(unsigned short addr, unsigned char val)
{
	putlookup[addr >> 8](addr, val);
}

static void push(unsigned short val)
{
	unsigned short sp = REGS16_R(regs, SP) - 2;
	REGS16_W(regs, SP, sp);
	mem_put(sp, val & 0xff);
	mem_put(sp+1, val >> 8);
}

static unsigned short pop(void)
{
	unsigned short res;
	unsigned short sp = REGS16_R(regs, SP);

	res  = mem_get(sp);
	res += mem_get(sp+1) << 8;
	REGS16_W(regs, SP, sp + 2);

	return res;
}

static unsigned char get_imm8(void)
{
	unsigned short pc = REGS16_R(regs, PC);
	unsigned char res;
	REGS16_W(regs, PC, pc + 1);
	res = mem_get(pc);
	DPRINTF("%02x", res);
	return res;
}

static unsigned short get_imm16(void)
{
	return get_imm8() + ((unsigned short)get_imm8() << 8);
}

static inline void print_reg(int i)
{
	if (i == 6) DPRINTF("[HL]"); /* indirect memory access by [HL] */
	else DPRINTF("%c", regnames[i]);
}

static unsigned char get_reg(int i)
{
	if (i == 6) /* indirect memory access by [HL] */
		return mem_get(REGS16_R(regs, HL));
	return REGS8_R(regs, i);
}

static void put_reg(int i, unsigned char val)
{
	if (i == 6) /* indirect memory access by [HL] */
		mem_put(REGS16_R(regs, HL), val);
	else REGS8_W(regs, i, val);
}

static void op_unknown(unsigned char op, struct opinfo *oi)
{
	printf(" Unknown opcode %02x.\n", op);
	exit(0);
}

static void op_set(unsigned char op)
{
	int reg = op & 7;
	int bit = (op >> 3) & 7;

	DPRINTF("\tSET %d, ", bit);
	print_reg(reg);
	put_reg(reg, get_reg(reg) | (1 << bit));
}

static void op_res(unsigned char op)
{
	int reg = op & 7;
	int bit = (op >> 3) & 7;

	DPRINTF("\tRES %d, ", bit);
	print_reg(reg);
	put_reg(reg, get_reg(reg) & ~(1 << bit));
}

static void op_bit(unsigned char op)
{
	int reg = op & 7;
	int bit = (op >> 3) & 7;

	DPRINTF("\tBIT %d, ", bit);
	print_reg(reg);
	regs.rn.f &= ~NF;
	regs.rn.f |= HF | ZF;
	regs.rn.f ^= ((get_reg(reg) << 8) >> (bit+1)) & ZF;
}

static void op_rl(unsigned char op, struct opinfo *oi)
{
	int reg = op & 7;
	unsigned short res;
	unsigned char val;

	DPRINTF(" %s ", oi->name);
	print_reg(reg);
	res  = val = get_reg(reg);
	res  = res << 1;
	res |= (((unsigned short)regs.rn.f & CF) >> 4);
	regs.rn.f = (val >> 7) << 4;
	if (res == 0) regs.rn.f |= ZF;
	put_reg(reg, res);
}

static void op_rla(unsigned char op, struct opinfo *oi)
{
	unsigned short res;

	DPRINTF(" %s", oi->name);
	res  = regs.rn.a;
	res  = res << 1;
	res |= (((unsigned short)regs.rn.f & CF) >> 4);
	regs.rn.f = (regs.rn.a >> 7) << 4;
	if (res == 0) regs.rn.f |= ZF;
	regs.rn.a = res;
}

static void op_rlc(unsigned char op, struct opinfo *oi)
{
	int reg = op & 7;
	unsigned short res;
	unsigned char val;

	DPRINTF(" %s ", oi->name);
	print_reg(reg);
	res  = val = get_reg(reg);
	res  = res << 1;
	res |= (val >> 7);
	regs.rn.f = (val >> 7) << 4;
	if (res == 0) regs.rn.f |= ZF;
	put_reg(reg, res);
}

static void op_rlca(unsigned char op, struct opinfo *oi)
{
	unsigned short res;

	DPRINTF(" %s", oi->name);
	res  = regs.rn.a;
	res  = res << 1;
	res |= (regs.rn.a >> 7);
	regs.rn.f = (regs.rn.a >> 7) << 4;
	if (res == 0) regs.rn.f |= ZF;
	regs.rn.a = res;
}

static void op_sla(unsigned char op, struct opinfo *oi)
{
	int reg = op & 7;
	unsigned short res;
	unsigned char val;

	DPRINTF(" %s ", oi->name);
	print_reg(reg);
	res  = val = get_reg(reg);
	res  = res << 1;
	regs.rn.f = ((val >> 7) << 4);
	if (res == 0) regs.rn.f |= ZF;
	put_reg(reg, res);
}

static void op_rr(unsigned char op, struct opinfo *oi)
{
	int reg = op & 7;
	unsigned short res;
	unsigned char val;

	DPRINTF(" %s ", oi->name);
	print_reg(reg);
	res  = val = get_reg(reg);
	res  = res >> 1;
	res |= (((unsigned short)regs.rn.f & CF) << 3);
	regs.rn.f = (val & 1) << 4;
	if (res == 0) regs.rn.f |= ZF;
	put_reg(reg, res);
}

static void op_rra(unsigned char op, struct opinfo *oi)
{
	unsigned short res;

	DPRINTF(" %s", oi->name);
	res  = regs.rn.a;
	res  = res >> 1;
	res |= (((unsigned short)regs.rn.f & CF) << 3);
	regs.rn.f = (regs.rn.a & 1) << 4;
	if (res == 0) regs.rn.f |= ZF;
	regs.rn.a = res;
}

static void op_rrc(unsigned char op, struct opinfo *oi)
{
	int reg = op & 7;
	unsigned short res;
	unsigned char val;

	DPRINTF(" %s ", oi->name);
	print_reg(reg);
	res  = val = get_reg(reg);
	res  = res >> 1;
	res |= (val << 7);
	regs.rn.f = (val & 1) << 4;
	if (res == 0) regs.rn.f |= ZF;
	put_reg(reg, res);
}

static void op_rrca(unsigned char op, struct opinfo *oi)
{
	unsigned short res;

	DPRINTF(" %s", oi->name);
	res  = regs.rn.a;
	res  = res >> 1;
	res |= (regs.rn.a << 7);
	regs.rn.f = (regs.rn.a & 1) << 4;
	if (res == 0) regs.rn.f |= ZF;
	regs.rn.a = res;
}

static void op_sra(unsigned char op, struct opinfo *oi)
{
	int reg = op & 7;
	unsigned short res;
	unsigned char val;

	DPRINTF(" %s ", oi->name);
	print_reg(reg);
	res  = val = get_reg(reg);
	res  = res >> 1;
	res |= (val & 0x80);
	regs.rn.f = ((val & 1) << 4);
	if (res == 0) regs.rn.f |= ZF;
	put_reg(reg, res);
}

static void op_srl(unsigned char op, struct opinfo *oi)
{
	int reg = op & 7;
	unsigned short res;
	unsigned char val;

	DPRINTF(" %s ", oi->name);
	print_reg(reg);
	res  = val = get_reg(reg);
	res  = res >> 1;
	regs.rn.f = ((val & 1) << 4);
	if (res == 0) regs.rn.f |= ZF;
	put_reg(reg, res);
}

static void op_swap(unsigned char op, struct opinfo *oi)
{
	int reg = op & 7;
	unsigned short res;
	unsigned char val;

	DPRINTF(" %s ", oi->name);
	print_reg(reg);
	val = get_reg(reg);
	res = (val >> 4) |
	      (val << 4);
	regs.rn.f = 0;
	if (res == 0) regs.rn.f |= ZF;
	put_reg(reg, res);
}

static struct opinfo cbops[8] = {
	OPINFO("\tRLC", &op_rlc),		/* opcode cb00-cb07 */
	OPINFO("\tRRC", &op_rrc),		/* opcode cb08-cb0f */
	OPINFO("\tRL", &op_rl),		/* opcode cb10-cb17 */
	OPINFO("\tRR", &op_rr),		/* opcode cb18-cb1f */
	OPINFO("\tSLA", &op_sla),		/* opcode cb20-cb27 */
	OPINFO("\tSRA", &op_sra),		/* opcode cb28-cb2f */
	OPINFO("\tSWAP", &op_swap),		/* opcode cb30-cb37 */
	OPINFO("\tSRL", &op_srl),		/* opcode cb38-cb3f */
};

static void op_cbprefix(unsigned char op, struct opinfo *oi)
{
	unsigned short pc = REGS16_R(regs, PC);

	REGS16_W(regs, PC, pc + 1);
	op = mem_get(pc);
	switch (op >> 6) {
		case 0: cbops[(op >> 3) & 7].fn(op, &cbops[(op >> 3) & 7]);
			return;
		case 1: op_bit(op); return;
		case 2: op_res(op); return;
		case 3: op_set(op); return;
	}
	printf(" Unknown CB subopcode %02x.\n", op);
	exit(0);
}

static void op_ld(unsigned char op, struct opinfo *oi)
{
	int src = op & 7;
	int dst = (op >> 3) & 7;

	DPRINTF(" %s  ", oi->name);
	print_reg(dst);
	DPRINTF(", ");
	print_reg(src);
	put_reg(dst, get_reg(src));
}

static void op_ld_imm(unsigned char op, struct opinfo *oi)
{
	int ofs = get_imm16();

	DPRINTF(" %s  A, [0x%04x]", oi->name, ofs);
	regs.rn.a = mem_get(ofs);
}

static void op_ld_ind16_a(unsigned char op, struct opinfo *oi)
{
	int ofs = get_imm16();

	DPRINTF(" %s  [0x%04x], A", oi->name, ofs);
	mem_put(ofs, regs.rn.a);
}

static void op_ld_ind16_sp(unsigned char op, struct opinfo *oi)
{
	int ofs = get_imm16();
	int sp = REGS16_R(regs, SP);

	DPRINTF(" %s  [0x%04x], SP", oi->name, ofs);
	mem_put(ofs, sp & 0xff);
	mem_put(ofs+1, sp >> 8);
}

static void op_ld_hlsp(unsigned char op, struct opinfo *oi)
{
	char ofs = get_imm8();
	unsigned short old = REGS16_R(regs, SP);
	unsigned short new = old + ofs;

	if (ofs>0) DPRINTF(" %s  HL, SP+0x%02x", oi->name, ofs);
	else DPRINTF(" %s  HL, SP-0x%02x", oi->name, -ofs);
	REGS16_W(regs, HL, new);
	regs.rn.f = 0;
	if (old > new) regs.rn.f |= CF;
	if ((old & 0xfff) > (new & 0xfff)) regs.rn.f |= HF;
}

static void op_ld_sphl(unsigned char op, struct opinfo *oi)
{
	DPRINTF(" %s  SP, HL", oi->name);
	REGS16_W(regs, SP, REGS16_R(regs, HL));
}

static void op_ld_reg16_imm(unsigned char op, struct opinfo *oi)
{
	int val = get_imm16();
	int reg = (op >> 4) & 3;

	reg += reg > 2; /* skip over FA */
	DPRINTF(" %s  %s, 0x%04x", oi->name, regnamech16[reg], val);
	REGS16_W(regs, reg, val);
}

static void op_ld_reg16_a(unsigned char op, struct opinfo *oi)
{
	int reg = (op >> 4) & 3;
	unsigned short r;

	reg -= reg > 2;
	if (op & 8) {
		DPRINTF(" %s  A, [%s]", oi->name, regnamech16[reg]);
		regs.rn.a = mem_get(r = REGS16_R(regs, reg));
	} else {
		DPRINTF(" %s  [%s], A", oi->name, regnamech16[reg]);
		mem_put(r = REGS16_R(regs, reg), regs.rn.a);
	}

	if (reg == 2) {
		r += (((op & 0x10) == 0) << 1)-1;
		REGS16_W(regs, reg, r);
	}
}

static void op_ld_reg8_imm(unsigned char op, struct opinfo *oi)
{
	int val = get_imm8();
	int reg = (op >> 3) & 7;

	DPRINTF(" %s  ", oi->name);
	print_reg(reg);
	put_reg(reg, val);
	DPRINTF(", 0x%02x", val);
}

static void op_ldh(unsigned char op, struct opinfo *oi)
{
	int ofs = op & 2 ? 0 : get_imm8();

	if (op & 0x10) {
		DPRINTF(" %s  A, ", oi->name);
		if ((op & 2) == 0) {
			DPRINTF("[%02x]", ofs);
		} else {
			ofs = regs.rn.c;
			DPRINTF("[C]");
		}
		regs.rn.a = mem_get(0xff00 + ofs);
	} else {
		if ((op & 2) == 0) {
			DPRINTF(" %s  [%02x], A", oi->name, ofs);
		} else {
			ofs = regs.rn.c;
			DPRINTF(" %s  [C], A", oi->name);
		}
		mem_put(0xff00 + ofs, regs.rn.a);
	}
}

static void op_inc(unsigned char op, struct opinfo *oi)
{
	int reg = (op >> 3) & 7;
	unsigned char res;
	unsigned char old;

	DPRINTF(" %s ", oi->name);
	print_reg(reg);
	old = res = get_reg(reg);
	res++;
	put_reg(reg, res);
	regs.rn.f &= ~(NF | ZF | HF);
	if (res == 0) regs.rn.f |= ZF;
	if ((old & 15) > (res & 15)) regs.rn.f |= HF;
}

static void op_inc16(unsigned char op, struct opinfo *oi)
{
	int reg = (op >> 4) & 3;
	unsigned short res = REGS16_R(regs, reg);

	DPRINTF(" %s %s\t", oi->name, regnamech16[reg]);
	res++;
	REGS16_W(regs, reg, res);
}

static void op_dec(unsigned char op, struct opinfo *oi)
{
	int reg = (op >> 3) & 7;
	unsigned char res;
	unsigned char old;

	DPRINTF(" %s ", oi->name);
	print_reg(reg);
	old = res = get_reg(reg);
	res--;
	put_reg(reg, res);
	regs.rn.f |= NF;
	regs.rn.f &= ~(ZF | HF);
	if (res == 0) regs.rn.f |= ZF;
	if ((old & 15) > (res & 15)) regs.rn.f |= HF;
}

static void op_dec16(unsigned char op, struct opinfo *oi)
{
	int reg = (op >> 4) & 3;
	unsigned short res = REGS16_R(regs, reg);

	DPRINTF(" %s %s", oi->name, regnamech16[reg]);
	res--;
	REGS16_W(regs, reg, res);
}

static void op_add_sp_imm(unsigned char op, struct opinfo *oi)
{
	char imm = get_imm8();
	unsigned short old = REGS16_R(regs, SP);
	unsigned short new = old;

	DPRINTF(" %s SP, %02x", oi->name, imm);
	new += imm;
	REGS16_W(regs, SP, new);
	regs.rn.f = 0;
	if (old > new) regs.rn.f |= CF;
	if ((old & 0xfff) > (new & 0xfff)) regs.rn.f |= HF;
}

static void op_add(unsigned char op, struct opinfo *oi)
{
	unsigned char old = regs.rn.a;
	unsigned char new;

	DPRINTF(" %s A, ", oi->name);
	print_reg(op & 7);
	regs.rn.a += get_reg(op & 7);
	new = regs.rn.a;
	regs.rn.f = 0;
	if (old > new) regs.rn.f |= CF;
	if ((old & 15) > (new & 15)) regs.rn.f |= HF;
	if (new == 0) regs.rn.f |= ZF;
}

static void op_add_imm(unsigned char op, struct opinfo *oi)
{
	unsigned char imm = get_imm8();
	unsigned char old = regs.rn.a;
	unsigned char new = old;

	DPRINTF(" %s A, $0x%02x", oi->name, imm);
	new += imm;
	regs.rn.a = new;
	regs.rn.f = 0;
	if (old > new) regs.rn.f |= CF;
	if ((old & 15) > (new & 15)) regs.rn.f |= HF;
	if (new == 0) regs.rn.f |= ZF;
}

static void op_add_hl(unsigned char op, struct opinfo *oi)
{
	int reg = (op >> 4) & 3;
	unsigned short old = REGS16_R(regs, HL);
	unsigned short new = old;

	reg += reg > 2;
	DPRINTF(" %s HL, %s", oi->name, regnamech16[reg]);

	new += REGS16_R(regs, reg);
	REGS16_W(regs, HL, new);

	regs.rn.f &= ~(NF | CF | HF);
	if (old > new) regs.rn.f |= CF;
	if ((old & 0xfff) > (new & 0xfff)) regs.rn.f |= HF;
}

static void op_adc(unsigned char op, struct opinfo *oi)
{
	unsigned char old = regs.rn.a;
	unsigned char new;

	DPRINTF(" %s A, ", oi->name);
	print_reg(op & 7);
	regs.rn.a += get_reg(op & 7);
	regs.rn.a += (regs.rn.f & CF) > 0;
	regs.rn.f &= ~NF;
	new = regs.rn.a;
	if (old > new) regs.rn.f |= CF; else regs.rn.f &= ~CF;
	if ((old & 15) > (new & 15)) regs.rn.f |= HF; else regs.rn.f &= ~HF;
	if (new == 0) regs.rn.f |= ZF; else regs.rn.f &= ~ZF;
}

static void op_adc_imm(unsigned char op, struct opinfo *oi)
{
	unsigned char imm = get_imm8();
	unsigned char old = regs.rn.a;
	unsigned char new = old;

	DPRINTF(" %s A, $0x%02x", oi->name, imm);
	new += imm;
	new += (regs.rn.f & CF) > 0;
	regs.rn.f &= ~NF;
	regs.rn.a = new;
	if (old > new) regs.rn.f |= CF; else regs.rn.f &= ~CF;
	if ((old & 15) > (new & 15)) regs.rn.f |= HF; else regs.rn.f &= ~HF;
	if (new == 0) regs.rn.f |= ZF; else regs.rn.f &= ~ZF;
}

static void op_cp(unsigned char op, struct opinfo *oi)
{
	unsigned char old = regs.rn.a;
	unsigned char new = old;

	DPRINTF(" %s A, ", oi->name);
	print_reg(op & 7);
	new -= get_reg(op & 7);
	regs.rn.f = NF;
	if (old < new) regs.rn.f |= CF;
	if ((old & 15) < (new & 15)) regs.rn.f |= HF;
	if (new == 0) regs.rn.f |= ZF;
}

static void op_cp_imm(unsigned char op, struct opinfo *oi)
{
	unsigned char imm = get_imm8();
	unsigned char old = regs.rn.a;
	unsigned char new = old;

	DPRINTF(" %s A, $0x%02x", oi->name, imm);
	new -= imm;
	regs.rn.f = NF;
	if (old < new) regs.rn.f |= CF;
	if ((old & 15) < (new & 15)) regs.rn.f |= HF;
	if (new == 0) regs.rn.f |= ZF;
}

static void op_sub(unsigned char op, struct opinfo *oi)
{
	unsigned char old = regs.rn.a;
	unsigned char new;

	DPRINTF(" %s A, ", oi->name);
	print_reg(op & 7);
	regs.rn.a -= get_reg(op & 7);
	new = regs.rn.a;
	regs.rn.f = NF;
	if (old < new) regs.rn.f |= CF;
	if ((old & 15) < (new & 15)) regs.rn.f |= HF;
	if (new == 0) regs.rn.f |= ZF;
}

static void op_sub_imm(unsigned char op, struct opinfo *oi)
{
	unsigned char imm = get_imm8();
	unsigned char old = regs.rn.a;
	unsigned char new = old;

	DPRINTF(" %s A, $0x%02x", oi->name, imm);
	new -= imm;
	regs.rn.a = new;
	regs.rn.f = NF;
	if (old < new) regs.rn.f |= CF;
	if ((old & 15) < (new & 15)) regs.rn.f |= HF;
	if (new == 0) regs.rn.f |= ZF;
}

static void op_sbc(unsigned char op, struct opinfo *oi)
{
	unsigned char old = regs.rn.a;
	unsigned char new;

	DPRINTF(" %s A, ", oi->name);
	print_reg(op & 7);
	regs.rn.a -= get_reg(op & 7);
	regs.rn.a -= (regs.rn.f & CF) > 0;
	new = regs.rn.a;
	regs.rn.f = NF;
	if (old < new) regs.rn.f |= CF;
	if ((old & 15) < (new & 15)) regs.rn.f |= HF;
	if (new == 0) regs.rn.f |= ZF;
}

static void op_sbc_imm(unsigned char op, struct opinfo *oi)
{
	unsigned char imm = get_imm8();
	unsigned char old = regs.rn.a;
	unsigned char new = old;

	DPRINTF(" %s A, $0x%02x", oi->name, imm);
	new -= imm;
	new -= (regs.rn.f & CF) > 0;
	regs.rn.a = new;
	regs.rn.f = NF;
	if (old < new) regs.rn.f |= CF;
	if ((old & 15) < (new & 15)) regs.rn.f |= HF;
	if (new == 0) regs.rn.f |= ZF;
}

static void op_and(unsigned char op, struct opinfo *oi)
{
	DPRINTF(" %s A, ", oi->name);
	print_reg(op & 7);
	regs.rn.a &= get_reg(op & 7);
	regs.rn.f = HF;
	if (regs.rn.a == 0) regs.rn.f |= ZF;
}

static void op_and_imm(unsigned char op, struct opinfo *oi)
{
	unsigned char imm = get_imm8();

	DPRINTF(" %s A, $0x%02x", oi->name, imm);
	regs.rn.a &= imm;
	regs.rn.f = HF;
	if (regs.rn.a == 0) regs.rn.f |= ZF;
}

static void op_or(unsigned char op, struct opinfo *oi)
{
	DPRINTF(" %s A, ", oi->name);
	print_reg(op & 7);
	regs.rn.a |= get_reg(op & 7);
	regs.rn.f = 0;
	if (regs.rn.a == 0) regs.rn.f |= ZF;
}

static void op_or_imm(unsigned char op, struct opinfo *oi)
{
	unsigned char imm = get_imm8();

	DPRINTF(" %s A, $0x%02x", oi->name, imm);
	regs.rn.a |= imm;
	regs.rn.f = 0;
	if (regs.rn.a == 0) regs.rn.f |= ZF;
}

static void op_xor(unsigned char op, struct opinfo *oi)
{
	DPRINTF(" %s A, ", oi->name);
	print_reg(op & 7);
	regs.rn.a ^= get_reg(op & 7);
	regs.rn.f = 0;
	if (regs.rn.a == 0) regs.rn.f |= ZF;
}

static void op_xor_imm(unsigned char op, struct opinfo *oi)
{
	unsigned char imm = get_imm8();

	DPRINTF(" %s A, $0x%02x", oi->name, imm);
	regs.rn.a ^= imm;
	regs.rn.f = 0;
	if (regs.rn.a == 0) regs.rn.f |= ZF;
}

static void op_push(unsigned char op, struct opinfo *oi)
{
	int reg = op >> 4 & 3;

	push(REGS16_R(regs, reg));
	DPRINTF(" %s %s\t", oi->name, regnamech16[reg]);
}

static void op_pop(unsigned char op, struct opinfo *oi)
{
	int reg = op >> 4 & 3;

	REGS16_W(regs, reg, pop());
	DPRINTF(" %s %s\t", oi->name, regnamech16[reg]);
}

static void op_cpl(unsigned char op, struct opinfo *oi)
{
	DPRINTF(" %s", oi->name);
	regs.rn.a = ~regs.rn.a;
	regs.rn.f |= NF | HF;
}

static void op_ccf(unsigned char op, struct opinfo *oi)
{
	DPRINTF(" %s", oi->name);
	regs.rn.f ^= CF;
	regs.rn.f &= ~(NF | HF);
}

static void op_scf(unsigned char op, struct opinfo *oi)
{
	DPRINTF(" %s", oi->name);
	regs.rn.f |= CF;
	regs.rn.f &= ~(NF | HF);
}

static void op_call(unsigned char op, struct opinfo *oi)
{
	unsigned short ofs = get_imm16();

	DPRINTF(" %s 0x%04x", oi->name, ofs);
	push(REGS16_R(regs, PC));
	REGS16_W(regs, PC, ofs);
}

static void op_call_cond(unsigned char op, struct opinfo *oi)
{
	unsigned short ofs = get_imm16();
	int cond = (op >> 3) & 3;

	DPRINTF(" %s %s 0x%04x", oi->name, conds[cond], ofs);
	switch (cond) {
		case 0: if ((regs.rn.f & ZF) != 0) return; break;
		case 1: if ((regs.rn.f & ZF) == 0) return; break;
		case 2: if ((regs.rn.f & CF) != 0) return; break;
		case 3: if ((regs.rn.f & CF) == 0) return; break;
	}
	push(REGS16_R(regs, PC));
	REGS16_W(regs, PC, ofs);
}

static void op_ret(unsigned char op, struct opinfo *oi)
{
	REGS16_W(regs, PC, pop());
	DPRINTF(" %s", oi->name);
}

static void op_reti(unsigned char op, struct opinfo *oi)
{
	REGS16_W(regs, PC, pop());
	DPRINTF(" %s", oi->name);
}

static void op_ret_cond(unsigned char op, struct opinfo *oi)
{
	int cond = (op >> 3) & 3;

	DPRINTF(" %s %s", oi->name, conds[cond]);
	switch (cond) {
		case 0: if ((regs.rn.f & ZF) != 0) return; break;
		case 1: if ((regs.rn.f & ZF) == 0) return; break;
		case 2: if ((regs.rn.f & CF) != 0) return; break;
		case 3: if ((regs.rn.f & CF) == 0) return; break;
	}
	REGS16_W(regs, PC, pop());
}

static void op_halt(unsigned char op, struct opinfo *oi)
{
	halted = 1;
	DPRINTF(" %s", oi->name);
}

static void op_stop(unsigned char op, struct opinfo *oi)
{
	DPRINTF(" %s", oi->name);
}

static void op_di(unsigned char op, struct opinfo *oi)
{
	interrupts = 0;
	DPRINTF(" %s", oi->name);
}

static void op_ei(unsigned char op, struct opinfo *oi)
{
	interrupts = 1;
	DPRINTF(" %s", oi->name);
}

static void op_jr(unsigned char op, struct opinfo *oi)
{
	short ofs = (char) get_imm8();

	if (ofs < 0) DPRINTF(" %s $-0x%02x", oi->name, -ofs);
	else DPRINTF(" %s $+0x%02x", oi->name, ofs);
	REGS16_W(regs, PC, REGS16_R(regs, PC) + ofs);
}

static void op_jr_cond(unsigned char op, struct opinfo *oi)
{
	short ofs = (char) get_imm8();
	int cond = (op >> 3) & 3;

	if (ofs < 0) DPRINTF(" %s %s $-0x%02x", oi->name, conds[cond], -ofs);
	else DPRINTF(" %s %s $+0x%02x", oi->name, conds[cond], ofs);
	switch (cond) {
		case 0: if ((regs.rn.f & ZF) != 0) return; break;
		case 1: if ((regs.rn.f & ZF) == 0) return; break;
		case 2: if ((regs.rn.f & CF) != 0) return; break;
		case 3: if ((regs.rn.f & CF) == 0) return; break;
	}
	REGS16_W(regs, PC, REGS16_R(regs, PC) + ofs);
}

static void op_jp(unsigned char op, struct opinfo *oi)
{
	unsigned short ofs = get_imm16();

	DPRINTF(" %s 0x%04x", oi->name, ofs);
	REGS16_W(regs, PC, ofs);
}

static void op_jp_hl(unsigned char op, struct opinfo *oi)
{
	DPRINTF(" %s HL", oi->name);
	REGS16_W(regs, PC, REGS16_R(regs, HL));
}

static void op_jp_cond(unsigned char op, struct opinfo *oi)
{
	unsigned short ofs = get_imm16();
	int cond = (op >> 3) & 3;

	DPRINTF(" %s %s 0x%04x", oi->name, conds[cond], ofs);
	switch (cond) {
		case 0: if ((regs.rn.f & ZF) != 0) return; break;
		case 1: if ((regs.rn.f & ZF) == 0) return; break;
		case 2: if ((regs.rn.f & CF) != 0) return; break;
		case 3: if ((regs.rn.f & CF) == 0) return; break;
	}
	REGS16_W(regs, PC, ofs);
}

static void op_rst(unsigned char op, struct opinfo *oi)
{
	short ofs = op & 0x38;

	DPRINTF(" %s 0x%02x", oi->name, ofs);
	push(REGS16_R(regs, PC));
	REGS16_W(regs, PC, ofs);
}

static void op_nop(unsigned char op, struct opinfo *oi)
{
	DPRINTF(" %s", oi->name);
}

static struct opinfo ops[256] = {
	OPINFO("\tNOP", &op_nop),		/* opcode 00 */
	OPINFO("\tLD", &op_ld_reg16_imm),		/* opcode 01 */
	OPINFO("\tLD", &op_ld_reg16_a),		/* opcode 02 */
	OPINFO("\tINC", &op_inc16),		/* opcode 03 */
	OPINFO("\tINC", &op_inc),		/* opcode 04 */
	OPINFO("\tDEC", &op_dec),		/* opcode 05 */
	OPINFO("\tLD", &op_ld_reg8_imm),		/* opcode 06 */
	OPINFO("\tRLCA", &op_rlca),		/* opcode 07 */
	OPINFO("\tLD", &op_ld_ind16_sp),		/* opcode 08 */
	OPINFO("\tADD", &op_add_hl),		/* opcode 09 */
	OPINFO("\tLD", &op_ld_reg16_a),		/* opcode 0a */
	OPINFO("\tDEC", &op_dec16),		/* opcode 0b */
	OPINFO("\tINC", &op_inc),		/* opcode 0c */
	OPINFO("\tDEC", &op_dec),		/* opcode 0d */
	OPINFO("\tLD", &op_ld_reg8_imm),		/* opcode 0e */
	OPINFO("\tRRCA", &op_rrca),		/* opcode 0f */
	OPINFO("\tSTOP", &op_stop),		/* opcode 10 */
	OPINFO("\tLD", &op_ld_reg16_imm),		/* opcode 11 */
	OPINFO("\tLD", &op_ld_reg16_a),		/* opcode 12 */
	OPINFO("\tINC", &op_inc16),		/* opcode 13 */
	OPINFO("\tINC", &op_inc),		/* opcode 14 */
	OPINFO("\tDEC", &op_dec),		/* opcode 15 */
	OPINFO("\tLD", &op_ld_reg8_imm),		/* opcode 16 */
	OPINFO("\tRLA", &op_rla),		/* opcode 17 */
	OPINFO("\tJR", &op_jr),		/* opcode 18 */
	OPINFO("\tADD", &op_add_hl),		/* opcode 19 */
	OPINFO("\tLD", &op_ld_reg16_a),		/* opcode 1a */
	OPINFO("\tDEC", &op_dec16),		/* opcode 1b */
	OPINFO("\tINC", &op_inc),		/* opcode 1c */
	OPINFO("\tDEC", &op_dec),		/* opcode 1d */
	OPINFO("\tLD", &op_ld_reg8_imm),		/* opcode 1e */
	OPINFO("\tRRA", &op_rra),		/* opcode 1f */
	OPINFO("\tJR", &op_jr_cond),		/* opcode 20 */
	OPINFO("\tLD", &op_ld_reg16_imm),		/* opcode 21 */
	OPINFO("\tLDI", &op_ld_reg16_a),		/* opcode 22 */
	OPINFO("\tINC", &op_inc16),		/* opcode 23 */
	OPINFO("\tINC", &op_inc),		/* opcode 24 */
	OPINFO("\tDEC", &op_dec),		/* opcode 25 */
	OPINFO("\tLD", &op_ld_reg8_imm),		/* opcode 26 */
	OPINFO("\tUNKN", &op_unknown),		/* opcode 27 */
	OPINFO("\tJR", &op_jr_cond),		/* opcode 28 */
	OPINFO("\tADD", &op_add_hl),		/* opcode 29 */
	OPINFO("\tLDI", &op_ld_reg16_a),		/* opcode 2a */
	OPINFO("\tDEC", &op_dec16),		/* opcode 2b */
	OPINFO("\tINC", &op_inc),		/* opcode 2c */
	OPINFO("\tDEC", &op_dec),		/* opcode 2d */
	OPINFO("\tLD", &op_ld_reg8_imm),		/* opcode 2e */
	OPINFO("\tCPL", &op_cpl),		/* opcode 2f */
	OPINFO("\tJR", &op_jr_cond),		/* opcode 30 */
	OPINFO("\tLD", &op_ld_reg16_imm),		/* opcode 31 */
	OPINFO("\tLDD", &op_ld_reg16_a),		/* opcode 32 */
	OPINFO("\tINC", &op_inc16),		/* opcode 33 */
	OPINFO("\tINC", &op_inc),		/* opcode 34 */
	OPINFO("\tDEC", &op_dec),		/* opcode 35 */
	OPINFO("\tLD", &op_ld_reg8_imm),		/* opcode 36 */
	OPINFO("\tSCF", &op_scf),		/* opcode 37 */
	OPINFO("\tJR", &op_jr_cond),		/* opcode 38 */
	OPINFO("\tADD", &op_add_hl),		/* opcode 39 */
	OPINFO("\tLDD", &op_ld_reg16_a),		/* opcode 3a */
	OPINFO("\tDEC", &op_dec16),		/* opcode 3b */
	OPINFO("\tINC", &op_inc),		/* opcode 3c */
	OPINFO("\tDEC", &op_dec),		/* opcode 3d */
	OPINFO("\tLD", &op_ld_reg8_imm),		/* opcode 3e */
	OPINFO("\tCCF", &op_ccf),		/* opcode 3f */
	OPINFO("\tLD", &op_ld),		/* opcode 40 */
	OPINFO("\tLD", &op_ld),		/* opcode 41 */
	OPINFO("\tLD", &op_ld),		/* opcode 42 */
	OPINFO("\tLD", &op_ld),		/* opcode 43 */
	OPINFO("\tLD", &op_ld),		/* opcode 44 */
	OPINFO("\tLD", &op_ld),		/* opcode 45 */
	OPINFO("\tLD", &op_ld),		/* opcode 46 */
	OPINFO("\tLD", &op_ld),		/* opcode 47 */
	OPINFO("\tLD", &op_ld),		/* opcode 48 */
	OPINFO("\tLD", &op_ld),		/* opcode 49 */
	OPINFO("\tLD", &op_ld),		/* opcode 4a */
	OPINFO("\tLD", &op_ld),		/* opcode 4b */
	OPINFO("\tLD", &op_ld),		/* opcode 4c */
	OPINFO("\tLD", &op_ld),		/* opcode 4d */
	OPINFO("\tLD", &op_ld),		/* opcode 4e */
	OPINFO("\tLD", &op_ld),		/* opcode 4f */
	OPINFO("\tLD", &op_ld),		/* opcode 50 */
	OPINFO("\tLD", &op_ld),		/* opcode 51 */
	OPINFO("\tLD", &op_ld),		/* opcode 52 */
	OPINFO("\tLD", &op_ld),		/* opcode 53 */
	OPINFO("\tLD", &op_ld),		/* opcode 54 */
	OPINFO("\tLD", &op_ld),		/* opcode 55 */
	OPINFO("\tLD", &op_ld),		/* opcode 56 */
	OPINFO("\tLD", &op_ld),		/* opcode 57 */
	OPINFO("\tLD", &op_ld),		/* opcode 58 */
	OPINFO("\tLD", &op_ld),		/* opcode 59 */
	OPINFO("\tLD", &op_ld),		/* opcode 5a */
	OPINFO("\tLD", &op_ld),		/* opcode 5b */
	OPINFO("\tLD", &op_ld),		/* opcode 5c */
	OPINFO("\tLD", &op_ld),		/* opcode 5d */
	OPINFO("\tLD", &op_ld),		/* opcode 5e */
	OPINFO("\tLD", &op_ld),		/* opcode 5f */
	OPINFO("\tLD", &op_ld),		/* opcode 60 */
	OPINFO("\tLD", &op_ld),		/* opcode 61 */
	OPINFO("\tLD", &op_ld),		/* opcode 62 */
	OPINFO("\tLD", &op_ld),		/* opcode 63 */
	OPINFO("\tLD", &op_ld),		/* opcode 64 */
	OPINFO("\tLD", &op_ld),		/* opcode 65 */
	OPINFO("\tLD", &op_ld),		/* opcode 66 */
	OPINFO("\tLD", &op_ld),		/* opcode 67 */
	OPINFO("\tLD", &op_ld),		/* opcode 68 */
	OPINFO("\tLD", &op_ld),		/* opcode 69 */
	OPINFO("\tLD", &op_ld),		/* opcode 6a */
	OPINFO("\tLD", &op_ld),		/* opcode 6b */
	OPINFO("\tLD", &op_ld),		/* opcode 6c */
	OPINFO("\tLD", &op_ld),		/* opcode 6d */
	OPINFO("\tLD", &op_ld),		/* opcode 6e */
	OPINFO("\tLD", &op_ld),		/* opcode 6f */
	OPINFO("\tLD", &op_ld),		/* opcode 70 */
	OPINFO("\tLD", &op_ld),		/* opcode 71 */
	OPINFO("\tLD", &op_ld),		/* opcode 72 */
	OPINFO("\tLD", &op_ld),		/* opcode 73 */
	OPINFO("\tLD", &op_ld),		/* opcode 74 */
	OPINFO("\tLD", &op_ld),		/* opcode 75 */
	OPINFO("\tHALT", &op_halt),		/* opcode 76 */
	OPINFO("\tLD", &op_ld),		/* opcode 77 */
	OPINFO("\tLD", &op_ld),		/* opcode 78 */
	OPINFO("\tLD", &op_ld),		/* opcode 79 */
	OPINFO("\tLD", &op_ld),		/* opcode 7a */
	OPINFO("\tLD", &op_ld),		/* opcode 7b */
	OPINFO("\tLD", &op_ld),		/* opcode 7c */
	OPINFO("\tLD", &op_ld),		/* opcode 7d */
	OPINFO("\tLD", &op_ld),		/* opcode 7e */
	OPINFO("\tLD", &op_ld),		/* opcode 7f */
	OPINFO("\tADD", &op_add),		/* opcode 80 */
	OPINFO("\tADD", &op_add),		/* opcode 81 */
	OPINFO("\tADD", &op_add),		/* opcode 82 */
	OPINFO("\tADD", &op_add),		/* opcode 83 */
	OPINFO("\tADD", &op_add),		/* opcode 84 */
	OPINFO("\tADD", &op_add),		/* opcode 85 */
	OPINFO("\tADD", &op_add),		/* opcode 86 */
	OPINFO("\tADD", &op_add),		/* opcode 87 */
	OPINFO("\tADC", &op_adc),		/* opcode 88 */
	OPINFO("\tADC", &op_adc),		/* opcode 89 */
	OPINFO("\tADC", &op_adc),		/* opcode 8a */
	OPINFO("\tADC", &op_adc),		/* opcode 8b */
	OPINFO("\tADC", &op_adc),		/* opcode 8c */
	OPINFO("\tADC", &op_adc),		/* opcode 8d */
	OPINFO("\tADC", &op_adc),		/* opcode 8e */
	OPINFO("\tADC", &op_adc),		/* opcode 8f */
	OPINFO("\tSUB", &op_sub),		/* opcode 90 */
	OPINFO("\tSUB", &op_sub),		/* opcode 91 */
	OPINFO("\tSUB", &op_sub),		/* opcode 92 */
	OPINFO("\tSUB", &op_sub),		/* opcode 93 */
	OPINFO("\tSUB", &op_sub),		/* opcode 94 */
	OPINFO("\tSUB", &op_sub),		/* opcode 95 */
	OPINFO("\tSUB", &op_sub),		/* opcode 96 */
	OPINFO("\tSUB", &op_sub),		/* opcode 97 */
	OPINFO("\tSBC", &op_sbc),		/* opcode 98 */
	OPINFO("\tSBC", &op_sbc),		/* opcode 99 */
	OPINFO("\tSBC", &op_sbc),		/* opcode 9a */
	OPINFO("\tSBC", &op_sbc),		/* opcode 9b */
	OPINFO("\tSBC", &op_sbc),		/* opcode 9c */
	OPINFO("\tSBC", &op_sbc),		/* opcode 9d */
	OPINFO("\tSBC", &op_sbc),		/* opcode 9e */
	OPINFO("\tSBC", &op_sbc),		/* opcode 9f */
	OPINFO("\tAND", &op_and),		/* opcode a0 */
	OPINFO("\tAND", &op_and),		/* opcode a1 */
	OPINFO("\tAND", &op_and),		/* opcode a2 */
	OPINFO("\tAND", &op_and),		/* opcode a3 */
	OPINFO("\tAND", &op_and),		/* opcode a4 */
	OPINFO("\tAND", &op_and),		/* opcode a5 */
	OPINFO("\tAND", &op_and),		/* opcode a6 */
	OPINFO("\tAND", &op_and),		/* opcode a7 */
	OPINFO("\tXOR", &op_xor),		/* opcode a8 */
	OPINFO("\tXOR", &op_xor),		/* opcode a9 */
	OPINFO("\tXOR", &op_xor),		/* opcode aa */
	OPINFO("\tXOR", &op_xor),		/* opcode ab */
	OPINFO("\tXOR", &op_xor),		/* opcode ac */
	OPINFO("\tXOR", &op_xor),		/* opcode ad */
	OPINFO("\tXOR", &op_xor),		/* opcode ae */
	OPINFO("\tXOR", &op_xor),		/* opcode af */
	OPINFO("\tOR", &op_or),		/* opcode b0 */
	OPINFO("\tOR", &op_or),		/* opcode b1 */
	OPINFO("\tOR", &op_or),		/* opcode b2 */
	OPINFO("\tOR", &op_or),		/* opcode b3 */
	OPINFO("\tOR", &op_or),		/* opcode b4 */
	OPINFO("\tOR", &op_or),		/* opcode b5 */
	OPINFO("\tOR", &op_or),		/* opcode b6 */
	OPINFO("\tOR", &op_or),		/* opcode b7 */
	OPINFO("\tCP", &op_cp),		/* opcode b8 */
	OPINFO("\tCP", &op_cp),		/* opcode b9 */
	OPINFO("\tCP", &op_cp),		/* opcode ba */
	OPINFO("\tCP", &op_cp),		/* opcode bb */
	OPINFO("\tCP", &op_cp),		/* opcode bc */
	OPINFO("\tCP", &op_cp),		/* opcode bd */
	OPINFO("\tCP", &op_cp),		/* opcode be */
	OPINFO("\tUNKN", &op_unknown),		/* opcode bf */
	OPINFO("\tRET", &op_ret_cond),		/* opcode c0 */
	OPINFO("\tPOP", &op_pop),		/* opcode c1 */
	OPINFO("\tJP", &op_jp_cond),		/* opcode c2 */
	OPINFO("\tJP", &op_jp),		/* opcode c3 */
	OPINFO("\tCALL", &op_call_cond),		/* opcode c4 */
	OPINFO("\tPUSH", &op_push),		/* opcode c5 */
	OPINFO("\tADD", &op_add_imm),		/* opcode c6 */
	OPINFO("\tRST", &op_rst),		/* opcode c7 */
	OPINFO("\tRET", &op_ret_cond),		/* opcode c8 */
	OPINFO("\tRET", &op_ret),		/* opcode c9 */
	OPINFO("\tJP", &op_jp_cond),		/* opcode ca */
	OPINFO("\tCBPREFIX", &op_cbprefix),		/* opcode cb */
	OPINFO("\tCALL", &op_call_cond),		/* opcode cc */
	OPINFO("\tCALL", &op_call),		/* opcode cd */
	OPINFO("\tADC", &op_adc_imm),		/* opcode ce */
	OPINFO("\tRST", &op_rst),		/* opcode cf */
	OPINFO("\tRET", &op_ret_cond),		/* opcode d0 */
	OPINFO("\tPOP", &op_pop),		/* opcode d1 */
	OPINFO("\tJP", &op_jp_cond),		/* opcode d2 */
	OPINFO("\tUNKN", &op_unknown),		/* opcode d3 */
	OPINFO("\tCALL", &op_call_cond),		/* opcode d4 */
	OPINFO("\tPUSH", &op_push),		/* opcode d5 */
	OPINFO("\tSUB", &op_sub_imm),		/* opcode d6 */
	OPINFO("\tRST", &op_rst),		/* opcode d7 */
	OPINFO("\tRET", &op_ret_cond),		/* opcode d8 */
	OPINFO("\tRETI", &op_reti),		/* opcode d9 */
	OPINFO("\tJP", &op_jp_cond),		/* opcode da */
	OPINFO("\tUNKN", &op_unknown),		/* opcode db */
	OPINFO("\tCALL", &op_call_cond),		/* opcode dc */
	OPINFO("\tUNKN", &op_unknown),		/* opcode dd */
	OPINFO("\tSBC", &op_sbc_imm),		/* opcode de */
	OPINFO("\tRST", &op_rst),		/* opcode df */
	OPINFO("\tLDH", &op_ldh),		/* opcode e0 */
	OPINFO("\tPOP", &op_pop),		/* opcode e1 */
	OPINFO("\tLDH", &op_ldh),		/* opcode e2 */
	OPINFO("\tUNKN", &op_unknown),		/* opcode e3 */
	OPINFO("\tUNKN", &op_unknown),		/* opcode e4 */
	OPINFO("\tPUSH", &op_push),		/* opcode e5 */
	OPINFO("\tAND", &op_and_imm),		/* opcode e6 */
	OPINFO("\tRST", &op_rst),		/* opcode e7 */
	OPINFO("\tADD", &op_add_sp_imm),		/* opcode e8 */
	OPINFO("\tJP", &op_jp_hl),		/* opcode e9 */
	OPINFO("\tLD", &op_ld_ind16_a),		/* opcode ea */
	OPINFO("\tUNKN", &op_unknown),		/* opcode eb */
	OPINFO("\tUNKN", &op_unknown),		/* opcode ec */
	OPINFO("\tUNKN", &op_unknown),		/* opcode ed */
	OPINFO("\tXOR", &op_xor_imm),		/* opcode ee */
	OPINFO("\tRST", &op_rst),		/* opcode ef */
	OPINFO("\tLDH", &op_ldh),		/* opcode f0 */
	OPINFO("\tPOP", &op_pop),		/* opcode f1 */
	OPINFO("\tLDH", &op_ldh),		/* opcode f2 */
	OPINFO("\tDI", &op_di),		/* opcode f3 */
	OPINFO("\tUNKN", &op_unknown),		/* opcode f4 */
	OPINFO("\tPUSH", &op_push),		/* opcode f5 */
	OPINFO("\tOR", &op_or_imm),		/* opcode f6 */
	OPINFO("\tRST", &op_rst),		/* opcode f7 */
	OPINFO("\tLD", &op_ld_hlsp),		/* opcode f8 */
	OPINFO("\tLD", &op_ld_sphl),		/* opcode f9 */
	OPINFO("\tLD", &op_ld_imm),		/* opcode fa */
	OPINFO("\tEI", &op_ei),		/* opcode fb */
	OPINFO("\tUNKN", &op_unknown),		/* opcode fc */
	OPINFO("\tUNKN", &op_unknown),		/* opcode fd */
	OPINFO("\tCP", &op_cp_imm),		/* opcode fe */
	OPINFO("\tRST", &op_rst),		/* opcode ff */
};

#if DEBUG == 1
static struct regs oldregs;

static void dump_regs(void)
{
	int i;

	DPRINTF("; ");
	for (i=0; i<8; i++) {
		DPRINTF("%c=%02x ", regnames[i], REGS8_R(regs, i));
	}
	for (i=5; i<6; i++) {
		DPRINTF("%s=%04x ", regnamech16[i], REGS16_R(regs, i));
	}
	DPRINTF("\n");
	oldregs = regs;
}

static void show_reg_diffs(void)
{
	int i;


	DPRINTF("\t\t; ");
	for (i=0; i<3; i++) {
		if (REGS16_R(regs, i) != REGS16_R(oldregs, i)) {
			DPRINTF("%s=%04x ", regnamech16[i], REGS16_R(regs, i));
			REGS16_W(oldregs, i, REGS16_R(regs, i));
		}
	}
	for (i=6; i<8; i++) {
		if (REGS8_R(regs, i) != REGS8_R(oldregs, i)) {
			if (i == 6) { /* Flags */
				if (regs.rn.f & ZF) DPRINTF("Z");
				else DPRINTF("z");
				if (regs.rn.f & NF) DPRINTF("N");
				else DPRINTF("n");
				if (regs.rn.f & HF) DPRINTF("H");
				else DPRINTF("h");
				if (regs.rn.f & CF) DPRINTF("C");
				else DPRINTF("c");
				DPRINTF(" ");
			} else {
				DPRINTF("%c=%02x ", regnames[i], REGS8_R(regs,i));
			}
			REGS8_W(oldregs, i, REGS8_R(regs, i));
		}
	}
	for (i=4; i<5; i++) {
		if (REGS16_R(regs, i) != REGS16_R(oldregs, i)) {
			DPRINTF("%s=%04x ", regnamech16[i], REGS16_R(regs, i));
			REGS16_W(oldregs, i, REGS16_R(regs, i));
		}
	}
	DPRINTF("\n");
}
#endif

void gbcpu_addmem(int start, int end, put_fn putfn, get_fn getfn)
{
	int i;

	for (i=start; i<=end; i++) {
		putlookup[i] = putfn;
		getlookup[i] = getfn;
	}
}

void gbcpu_init(void)
{
	DEB(dump_regs());
}

void gbcpu_intr(int vec)
{
	halted = 0;
	push(REGS16_R(regs, PC));
	REGS16_W(regs, PC, vec);
}

int gbcpu_step(void)
{
	unsigned char op;

	if (!halted) {
		op = mem_get(regs.rn.pc++);
		DPRINTF("%04x: %02x", regs.rn.pc - 1, op);
		ops[op].fn(op, &ops[op]);

		DEB(show_reg_diffs());
		return 1;
	}
	return 16;
}