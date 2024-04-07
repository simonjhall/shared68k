.section ".vectors", "ax"
.global pre_start
pre_start:
	la x1, _fault
	csrw mtvec, x1
	li sp, 0x100FFFFC

#todo clean up this stuff and somehow use defines
	la t0, 0x1000400
	lw t0, 0(t0)
	andi t0, t0, 2
	bne t0, zero, skip_zero

	la t0, 0x10000000 | 0x100
	la t1, 0x10000000 | (16 * 1024 * 1024)
1:
	sw zero, 0(t0)
	sw zero, 4(t0)
	sw zero, 8(t0)
	sw zero, 12(t0)

	sw zero, 16(t0)
	sw zero, 20(t0)
	sw zero, 24(t0)
	sw zero, 28(t0)

	add t0, t0, 32

	bne t0, t1, 1b
skip_zero:

	jal _start

.global skip_strip
skip_strip:
	ret
	
#if __riscv_xlen == 32
.macro STORE reg
	addi sp, sp, -4
	sw \reg, 0(sp)
.endm

.macro LOAD reg
	lw \reg, 0(sp)
	addi sp, sp, 4
.endm

#elif __riscv_xlen == 64
	todo update for rv64, including below
#endif

_fault:
	csrw mscratch, sp
	li sp, 0x100FFFFC
	
	STORE x31
	STORE x30
	STORE x29
	STORE x28
	
	STORE x27
	STORE x26
	STORE x25
	STORE x24
	
	STORE x23
	STORE x22
	STORE x21
	STORE x20
	
	STORE x19
	STORE x18
	STORE x17
	STORE x16
	
	STORE x15
	STORE x14
	STORE x13
	STORE x12
	
	STORE x11
	STORE x10
	STORE x9
	STORE x8
	
	STORE x7
	STORE x6
	STORE x5
	STORE x4
	
	STORE x3
	STORE x2
	STORE x1
	STORE x0
	
	csrr t0, mscratch
	nop
	sw t0, 32*4(sp)
	
	csrr t0, mepc
	nop
	sw t0, 33*4(sp)
	
	csrr t0, mstatus
	nop
	sw t0, 34*4(sp)
	
	mv a0, sp
	addi sp, sp, -4
	csrr a1, mcause
	
	jal interrupt_exception
	
	addi sp, sp, 4
	
	lw t0, 34*4(sp)
	csrw mstatus, t0
	
	lw t0, 33*4(sp)
	csrw mepc, t0
	
	lw t0, 32*4(sp)
	csrw mscratch, t0
	
	LOAD x0
	LOAD x1
	LOAD x2
	LOAD x3
	
	LOAD x4
	LOAD x5
	LOAD x6
	LOAD x7
	
	LOAD x8
	LOAD x9
	LOAD x10
	LOAD x11
	
	LOAD x12
	LOAD x13
	LOAD x14
	LOAD x15
	
	LOAD x16
	LOAD x17
	LOAD x18
	LOAD x19
	
	LOAD x20
	LOAD x21
	LOAD x22
	LOAD x23
	
	LOAD x24
	LOAD x25
	LOAD x26
	LOAD x27
	
	LOAD x28
	LOAD x29
	LOAD x30
	LOAD x31
	
	csrr sp, mscratch
	
	mret

