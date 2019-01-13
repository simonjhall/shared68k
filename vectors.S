.section ".vectors", "ax"

#define PRINT_ENABLE

#big ham origins
/*#define UART_BASE	(((1 << 21) | (1 << 20) | (1 << 19)))*/
#big daddy pro
/*#define UART_BASE	((0b001 << 21))*/
#dynamic daddy
/*#define UART_BASE	((1 << 15))*/

.global vector_table
.type vector_table,%object
vector_table:
#0-3
#origins
#.long 0x2ffff8
#big daddy pro
#.long 0x4FFFF8
#fpga 68008
.long 0x17ff8
#dynamic daddy
#.long  0xBFFFF8
.long pre_start
#.long pre_start_dynamic
.long _bus_error
.long _addr_error

#4-7
.long _illegal_instruction
.long _div_zero
.long _misc_trap
.long _misc_trap

#8-11
.long _misc_trap
.long _trace
.long _misc_trap
.long _misc_trap

#12-15
.long _misc_trap
.long _misc_trap
.long _misc_trap
.long _misc_trap

#16-23
.long _misc_trap
.long _misc_trap
.long _misc_trap
.long _misc_trap
.long _misc_trap
.long _misc_trap
.long _misc_trap
.long _misc_trap

#23-27
.long _spurious_int
.long _auto_1
.long _auto_2
.long _auto_3
.long _auto_4
.long _auto_5
.long _auto_6
.long _auto_7

#32-47
.long _trap
.long _trap
.long _trap
.long _trap
.long _trap
.long _trap
.long _trap
.long _trap
.long _trap
.long _trap
.long _trap
.long _trap
.long _trap
.long _trap
.long _trap
.long _trap

.size vector_table,.-vector_table

pre_start_dynamic:
#ifdef PRINT_ENABLE
	movea.l #(1 << 15), %a0

	lea.l in_pre_start_dynamic, %a1

pre_start_dynamic_loop:
	move.b %a1@+, %d0
	tst %d0
	move.b %d0, %a0@

	bne pre_start_dynamic_loop
#endif

	#precharge all
	movea.l #((1 << 16)+(1 << 19)), %a0
	move.b %a0@, %d0

	#load mode
	movea.l #((1 << 16)+(2 << 4)), %a0
	move.b %d0, %a0@

#ifdef PRINT_ENABLE
	movea.l #(1 << 15), %a0
	movea.l #0x800000, %a1
	move.l #255, %d2

	move.b #' ', %a0@

loop:
	#set the byte value to something interesting
	move.b %d2, %a1@
	#get the byte value
	move.b %a1@+, %d0

	#get the low and high nibble, 0-f
	move.l %d0, %d1
	andi.l #0xf, %d0
	andi.l #0xf0, %d1
	lsr.l #4, %d1

	#get into ascii range 0-9 or a-f
	cmpi.l #0xa, %d0
	blt 1f

	addi.l #('a'-10), %d0
	jmp 2f

1:
	addi.l #'0', %d0

2:
	cmpi.l #0xa, %d1
	blt 1f

	addi.l #('a'-10), %d1
	jmp 2f

1:
	addi.l #'0', %d1

2:
	#now print the value
	move.b %d1, %a0@
	move.b %d0, %a0@


	#formatting
	move.l %d2, %d0
	andi.l #15, %d0

	beq 1f
	jmp 2f
1:
	move.b #'\n', %a0@

2:	andi.l #3, %d0

	beq 1f
	jmp 2f
1:
	move.b #' ', %a0@

2:

	#next character
	subi.l #1, %d2

	bne loop

	#new line
	move.b #'\n', %a0@

#endif

pre_start:
#ifdef PRINT_ENABLE
	movea.l #(1 << 15), %a0

	lea.l in_pre_start, %a1

pre_start_loop:
	move.b %a1@+, %d0
	tst %d0
	move.b %d0, %a0@

	bne pre_start_loop
#endif
	jmp _start

.global skip_strip
skip_strip:
	rts

.macro fault_pre
	#a registers, then d registers from a0->6, d0->7
	movem.l %a6-%a0, -(%sp)
	movem.l %d7-%d0, -(%sp)

	#the user stack pointer
	move.l %usp, %a0
	movem.l %a0, -(%sp)

	#the address of all this
	sub.l #4, %sp
	move.l %sp, %sp@
	#and correct the value we stored
	add.l #4, %sp@
.endm

.macro fault_post

	#now correct to point at usp
	add.l #4, %sp

	#restore usp
	movem.l (%sp)+, %a0
	move.l %a0, %usp

	#restore d registers
	movem.l (%sp)+, %d0-%d7

	#and some a
	movem.l (%sp)+, %a0-%a6

	rte
.endm

_bus_error:
	fault_pre
	jsr bus_error
	fault_post

_addr_error:
	fault_pre
	jsr addr_error
	fault_post

_illegal_instruction:
	fault_pre
	jsr illegal_instruction
	fault_post

_div_zero:
	fault_pre
	jsr div_zero
	fault_post

_trace:
	fault_pre
	jsr trace
	fault_post

_misc_trap:
	fault_pre
	jsr misc_trap
	fault_post

_spurious_int:
	fault_pre
	jsr spurious_int
	fault_post

_auto_1:
	fault_pre
	jsr auto_1
	fault_post

_auto_2:
	fault_pre
	jsr auto_2
	fault_post

_auto_3:
	fault_pre
	jsr auto_3
	fault_post

_auto_4:
	fault_pre
	jsr auto_4
	fault_post

_auto_5:
	fault_pre
	jsr auto_5
	fault_post

_auto_6:
	fault_pre
	jsr auto_6
	fault_post

_auto_7:
	fault_pre
	jsr auto_7
	fault_post

_trap:
	fault_pre
	jsr trap
	fault_post


in_pre_start_dynamic:
	.asciz "in pre_start_dynamic\n"
in_pre_start:
	.asciz "in pre_start\n"
