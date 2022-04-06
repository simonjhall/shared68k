/*
 * misc_asm.cpp
 *
 *  Created on: 06 Feb 2022
 *      Author: simon
 */

#include "misc_asm.h"
#include "uart.h"
#include "common.h"

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

void invalidate_icache(void)
{
}

void enable_icache(bool)
{
}

