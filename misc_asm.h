/*
 * misc_asm.h
 *
 *  Created on: 29 Aug 2018
 *      Author: simon
 */

#ifndef MISC_ASM_H_
#define MISC_ASM_H_

unsigned int read_cacr(void);
void write_cacr(unsigned int value);
void write_sr(unsigned int value);
unsigned int read_sr(void);

void invalidate_icache(void);
void enable_icache(bool);
void enable_dcache(bool);

bool AreInterruptsEnabled(void);

void CallUserModeNoReturn(void (*pFunc)(void), unsigned short sr, void *pStack);

struct Mutex
{
	volatile unsigned int i;
};

void mutex_init(Mutex *p);
void mutex_lock(Mutex *p);
void mutex_unlock(Mutex *p);
int mutex_trylock(Mutex *p);

unsigned int trap0(unsigned int id);
unsigned int trap1(unsigned int id, unsigned int arg1);
unsigned int trap2(unsigned int id, unsigned int arg1, unsigned int arg2);

#endif /* MISC_ASM_H_ */
