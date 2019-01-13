/*
 * inter_process.h
 *
 *  Created on: 29 Aug 2018
 *      Author: simon
 */

#ifndef INTER_PROCESS_H_
#define INTER_PROCESS_H_

#define TRAP_PRINT_CHAR				0
#define TRAP_PRINT_HEX_NUM			1
#define TRAP_PRINT_HEX_BYTE			2
#define TRAP_PRINT_DEC_SHORT_NUM	3
#define TRAP_PRINT_STRING			4

#define DEBUGGER_UPDATE 100

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

Hooks *GetHooks(void);

#endif /* INTER_PROCESS_H_ */
