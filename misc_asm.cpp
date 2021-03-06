/*
 * misc_asm.cpp
 *
 *  Created on: 29 Aug 2018
 *      Author: simon
 */

#include "misc_asm.h"
#include "uart.h"
#include "common.h"


void write_cacr(unsigned int value)
{
#ifdef __mc68030__
	__asm__ __volatile__ (
						"move.l %0, %%d0\n\t"
						"movc %%d0, %%cacr\n\t"
					:
					: "g" (value)
					: "d0");
#endif
}


unsigned int read_cacr(void)
{
#if __mc68030__
	unsigned int out;
	__asm__ __volatile(
			"movc %%cacr, %%d0\n\t"
			"move.l %%d0, %0\n\t"
			: "=r" (out)
			:
			: "d0");
	return out;
#else
	return 0;
#endif
}

#ifdef __m68k__
void write_sr(unsigned int value)
{
	__asm__ __volatile__ (
						"move.l %0, %%d0\n\t"
						"move %%d0, %%sr\n\t"
					:
					: "g" (value)
					: "d0");
}
#endif

#ifdef __m68k__
unsigned int read_sr(void)
{
	unsigned int out;
	__asm__ __volatile__ (
						"move %%sr, %%d0\n\t"
						"move.l %%d0, %0\n\t"
					: "=r" (out)
					:
					: "d0");

	return out;
}
#endif

void invalidate_icache(void)
{
	write_cacr(read_cacr() | (1 << 3));
}

void enable_icache(bool e)
{
	if (e)
		write_cacr(read_cacr() | (1 << 0));
	else
		write_cacr(read_cacr() & ~(1 << 0));
}

void enable_dcache(bool e)
{
	if (e)
		write_cacr(read_cacr() | (1 << 8));
	else
		write_cacr(read_cacr() & ~(1 << 8));
}

bool AreInterruptsEnabled(void)
{
#ifdef __m68k__
	if ((read_sr() & (0b111 << 8)) == (0b111 << 8))
		return false;
	else
		return true;
#else
	return false;
#endif
}

bool IsSupervisorMode(void)
{
#ifdef __m68k__
	if (read_sr() & (1 << 13))
		return true;
	else
		return false;
#else
	return true;
#endif
}

void CallUserModeNoReturn(void (*pFunc)(void), unsigned short sr, void *pStack)
{
#ifdef __m68k__
	volatile struct Return
	{
		volatile unsigned short m_sr;
		volatile unsigned int m_pc;
	} ret;

	ret.m_sr = sr;
	ret.m_pc = (unsigned int)pFunc;

	__asm__ __volatile (
					"move.l %1, %%a0\n\t"
					"move.l %%a0, %%usp\n\t"
					"move.l %0, %%ssp\n\t"
					"rte\n\t"
					:
					: "g" (&ret), "g" (pStack)
					: "a0", "sp");
#else
	ASSERT(!"invalid CallUserModeNoReturn");
#endif
}

unsigned int trap0(unsigned int id)
{
#ifdef __m68k__
	unsigned int out;
	__asm__ __volatile__ (
						"move.l %1, %%d0\n\t"
						"trap #0\n\t"
						"move.l %%d0, %0\n\t"
					: "=r" (out)
					: "g" (id)
					: "d0");

	return out;
#else
	ASSERT(!"invalid trap0");
	return 0;
#endif
}

unsigned int trap1(unsigned int id, unsigned int arg1)
{
#ifdef __m68k__
	unsigned int out;
	__asm__ __volatile__ (
						"move.l %1, %%d0\n\t"
						"move.l %2, %%d1\n\t"
						"trap #0\n\t"
						"move.l %%d0, %0\n\t"
					: "=r" (out)
					: "g" (id), "g" (arg1)
					: "d0", "d1");

	return out;
#else
	ASSERT(!"invalid trap1");
	return 0;
#endif
}

unsigned int trap2(unsigned int id, unsigned int arg1, unsigned int arg2)
{
#ifdef __m68k__
	unsigned int out;
	__asm__ __volatile__ (
						"move.l %1, %%d0\n\t"
						"move.l %2, %%d1\n\t"
						"move.l %3, %%d2\n\t"
						"trap #0\n\t"
						"move.l %%d0, %0\n\t"
					: "=r" (out)
					: "g" (id), "g" (arg1), "g" (arg2)
					: "d0", "d1", "d2");

	return out;
#else
	ASSERT(!"invalid trap2");
	return 0;
#endif
}

