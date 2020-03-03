.section ".vectors", "ax"

#define PRINT_ENABLE

#redux
/*#define UART_BASE 0x1000580*/

.global vector_table
.type vector_table,%object
vector_table:
#redux
.long 0x17ff8
.long _start
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

.global skip_strip
skip_strip:
	ret

.macro fault_pre

.endm

.macro fault_post

.endm

_bus_error:
	fault_pre
	jal bus_error
	fault_post

_addr_error:
	fault_pre
	jal addr_error
	fault_post

_illegal_instruction:
	fault_pre
	jal illegal_instruction
	fault_post

_div_zero:
	fault_pre
	jal div_zero
	fault_post

_trace:
	fault_pre
	jal trace
	fault_post

_misc_trap:
	fault_pre
	jal misc_trap
	fault_post

_spurious_int:
	fault_pre
	jal spurious_int
	fault_post

_auto_1:
	fault_pre
	jal auto_1
	fault_post

_auto_2:
	fault_pre
	jal auto_2
	fault_post

_auto_3:
	fault_pre
	jal auto_3
	fault_post

_auto_4:
	fault_pre
	jal auto_4
	fault_post

_auto_5:
	fault_pre
	jal auto_5
	fault_post

_auto_6:
	fault_pre
	jal auto_6
	fault_post

_auto_7:
	fault_pre
	jal auto_7
	fault_post

_trap:
	fault_pre
	jal trap
	fault_post

