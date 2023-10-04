/*
 * misc_asm.cpp
 *
 *  Created on: 06 Feb 2022
 *      Author: simon
 */

#include <stdint.h>

#include "misc_asm.h"
#include "uart.h"
#include "common.h"
#include "inter_process.h"

unsigned int trap0(unsigned int id)
{
	unsigned int error, value;
	__asm__ __volatile__ (
						"li  a7, 0x09000000\n\t"
						"mv a6, %2\n\t"
						"ecall\n\t"
						"mv %0, a0\n\t"
						"mv %1, a1\n\t"
					: "=r" (error), "=r" (value)
					: "r" (id)
					: "a7", "a6", "a0", "a1");

	ASSERT(error == 1);
	return value;
}

unsigned int trap1(unsigned int id, unsigned int arg1)
{
	unsigned int error, value;
	__asm__ __volatile__ (
						"li  a7, 0x09000000\n\t"
						"mv a6, %2\n\t"
						"mv a0, %3\n\t"
						"ecall\n\t"
						"mv %0, a0\n\t"
						"mv %1, a1\n\t"
					: "=r" (error), "=r" (value)
					: "r" (id), "r" (arg1)
					: "a7", "a6", "a0", "a1");

	ASSERT(error == 1);
	return value;
}

unsigned int trap2(unsigned int id, unsigned int arg1, unsigned int arg2)
{
	unsigned int error, value;
	__asm__ __volatile__ (
						"li  a7, 0x09000000\n\t"
						"mv a6, %2\n\t"
						"mv a0, %3\n\t"
						"mv a1, %4\n\t"
						"ecall\n\t"
						"mv %0, a0\n\t"
						"mv %1, a1\n\t"
					: "=r" (error), "=r" (value)
					: "r" (id), "r" (arg1), "r" (arg2)
					: "a7", "a6", "a0", "a1");

	ASSERT(error == 1);
	return value;
}

void CallUserModeNoReturn(void (*pFunc)(void), unsigned long sr, void *pStack)
{
	__asm__ __volatile (
					"mv sp, %2\n\t"
					"mv t0, %1\n\t"
					"csrw mstatus, t0\n\t"
					"mv t0, %0\n\t"
					"csrw mepc, t0\n\t"
					"nop\n\t"
					"mret\n\t"
					:
					: "r" (pFunc), "r" (sr), "r" (pStack));
}

//https://stackoverflow.com/questions/18350021/how-to-print-exact-value-of-the-program-counter-in-c
__attribute__ ((__noinline__))
static uintptr_t get_pc () { return (uintptr_t)__builtin_return_address(0); }

void flush_dcache(bool invalidate)
{
#ifdef HAS_DCACHE
	if (get_pc() < ((uintptr_t)ROM_BASE + ROM_SIZE))
	{
		const unsigned int flush_shift = 10;
		const unsigned int invalidate_shift = 11;
		const unsigned int line_log = 10;

		//update each line
		for (unsigned int count = 0; count < (1 << line_log); count++)
		{
			unsigned int control = ((invalidate ? 1 : 0) << invalidate_shift) | (1 << flush_shift) | count;

			__asm__ __volatile__ (
								"csrw 0x7c1, %0\n"
							: 
							: "r" (control)
							: );
		}

		unsigned int disable = 0;

		__asm__ __volatile__ (
							"csrw 0x7c1, %0\n"
						: 
						: "r" (disable)
						: );
	}
	else
		GetHooks()->FlushDCache(invalidate);
#endif
}

void invalidate_icache(void)
{
#ifdef HAS_ICACHE
	if (get_pc() < ((uintptr_t)ROM_BASE + ROM_SIZE))
	{
		flush_dcache(false);

		const unsigned int valid_shift = 10;
		const unsigned int sel_shift = 11;
		const unsigned int line_log = 10;

		//clear each line
		for (unsigned int count = 0; count < (1 << line_log); count++)
		{
			unsigned int control = (0 << valid_shift) | (1 << sel_shift) | count;

			__asm__ __volatile__ (
								"csrw 0x7c0, %0\n"
							: 
							: "r" (control)
							: );
		}

		//turn it back on
		unsigned int enable = 1 << valid_shift;

		__asm__ __volatile__ (
							"csrw 0x7c0, %0\n"
						: 
						: "r" (enable)
						: );
	}
	else
		GetHooks()->InvalidateICache();
#endif
}

void enable_icache(bool e)
{
#ifdef HAS_ICACHE
	if (get_pc() < ((uintptr_t)ROM_BASE + ROM_SIZE))
	{
		ASSERT(e);			//can only enable

		const unsigned int valid_shift = 10;
		const unsigned int sel_shift = 11;
		unsigned int enable = 1 << valid_shift;

		__asm__ __volatile__ (
							"csrw 0x7c0, %0\n"
						: 
						: "r" (enable)
						: );
	}
	else
		GetHooks()->EnableICache(e);
#endif
}

unsigned int read_mie(void)
{
	unsigned int value;
	__asm__ __volatile__ (
						"csrr %0, mie\n"
					: "=r" (value)
					: 
					: );

	return value;
}

unsigned int read_mip(void)
{
	unsigned int value;
	__asm__ __volatile__ (
						"csrr %0, mip\n"
					: "=r" (value)
					: 
					: );

	return value;
}

unsigned int read_mstatus(void)
{
	unsigned int value;
	__asm__ __volatile__ (
						"csrr %0, mstatus\n"
					: "=r" (value)
					: 
					: );

	return value;
}

void write_mie(unsigned int mask)
{
	__asm__ __volatile__ (
						"csrw mie, %0\n"
					: 
					: "r" (mask)
					: );
}

void write_mstatus(unsigned int status)
{
	__asm__ __volatile__ (
						"csrw mstatus, %0\n"
					: 
					: "r" (status)
					: );
}

