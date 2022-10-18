/*
 * inter_process.h
 *
 *  Created on: 29 Aug 2018
 *      Author: simon
 */

#ifndef INTER_PROCESS_H_
#define INTER_PROCESS_H_

#include <stdint.h>

#define TRAP_PRINT_CHAR				1000
#define TRAP_PRINT_HEX_NUM			1001
#define TRAP_PRINT_HEX_BYTE			1002
#define TRAP_PRINT_DEC_SHORT_NUM	1003
#define TRAP_PRINT_STRING			1004

#define DEBUGGER_UPDATE 1100

#ifdef __m68k__
struct ExceptionState
{
	unsigned int usp;
	unsigned int d[8];
	unsigned int a[7];
	//and then the cpu-pushed state

	//does not work for address/bus exceptions on the '000
	unsigned short *GetSr(void)
	{
		return (unsigned short *)(this + 1);
	}

	unsigned int *GetPc(void)
	{
		return (unsigned int *)(((unsigned short *)(this + 1)) + 1);
	}

	unsigned short *GetGroup0Sr(void)
	{
		return (unsigned short *)(this + 1) + 4;
	}

	unsigned int *GetGroup0Pc(void)
	{
		return (unsigned int *)(((unsigned short *)(this + 1)) + 5);
	}
};

static const unsigned int s_exceptionStateSize = 16 * 4;
static_assert(sizeof(ExceptionState) == s_exceptionStateSize, "structure size change");

struct Hooks
{
	void (*BusError)(ExceptionState *pState);
	void (*AddrError)(ExceptionState *pState);
	void (*IllegalInst)(ExceptionState *pState);
	void (*DivZero)(ExceptionState *pState);
	void (*Trace)(ExceptionState *pState);
	void (*MiscTrap)(ExceptionState *pState);
	void (*SpuriousInt)(ExceptionState *pState);
	void (*Auto1)(ExceptionState *pState);
	void (*Auto2)(ExceptionState *pState);
	void (*Auto3)(ExceptionState *pState);
	void (*Auto4)(ExceptionState *pState);
	void (*Auto5)(ExceptionState *pState);
	void (*Auto6)(ExceptionState *pState);
	void (*Auto7)(ExceptionState *pState);
	bool (*Trap)(ExceptionState *pState);
};

static const unsigned int s_hooksSize = 15 * 4;
static_assert(sizeof(Hooks) == s_hooksSize, "structure size change");

#elif __riscv

struct ExceptionState
{
	//no state is saved by the cpu - this is all written by code
	unsigned long regs_int[32];
	unsigned long sp;
	unsigned long pc;
	unsigned long status;
	
	unsigned long *GetPc(void)
	{
		return &pc; 
	}
	
	unsigned long *GetSr(void)
	{
		return &status;
	}
};

static const unsigned int s_exceptionStateSize = 35 * 4;
static_assert(sizeof(ExceptionState) == s_exceptionStateSize, "structure size change");

struct Hooks
{
	//interrupts
	void (*SuperSoftInt)(ExceptionState *pState);
	void (*MachSoftInt)(ExceptionState *pState);
	void (*SuperTimerInt)(ExceptionState *pState);
	void (*MachTimerInt)(ExceptionState *pState);
	void (*SuperExtInt)(ExceptionState *pState);
	void (*MachExtInt)(ExceptionState *pState);
	
	//exceptions
	void (*InstAddrMisaligned)(ExceptionState *pState);
	void (*InstAddrFault)(ExceptionState *pState);
	void (*IllegalInst)(ExceptionState *pState);
	void (*Breakpoint)(ExceptionState *pState);
	void (*LoadAddrMisaligned)(ExceptionState *pState);
	void (*LoadAddrFault)(ExceptionState *pState);
	void (*StoreAddrMisaligned)(ExceptionState *pState);
	void (*StoreAddrFault)(ExceptionState *pState);
	void (*EcallFromU)(ExceptionState *pState);
	void (*EcallFromS)(ExceptionState *pState);
	void (*EcallFromM)(ExceptionState *pState);
	void (*InstPageFault)(ExceptionState *pState);
	void (*LoadPageFault)(ExceptionState *pState);
	void (*StorePageFault)(ExceptionState *pState);
};

static const uintptr_t s_hooksSize = 20 * sizeof(uintptr_t);
static_assert(sizeof(Hooks) == s_hooksSize, "structure size change");

#else
#error unexpected platform
#endif

static inline Hooks *GetHooks(void)
{
	return (Hooks *)RAM_BASE;
}

#endif /* INTER_PROCESS_H_ */
