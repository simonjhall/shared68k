/*
 * uart.cpp
 *
 *  Created on: 29 Aug 2018
 *      Author: simon
 */

#include "misc_asm.h"
#include "inter_process.h"
#include "common.h"

#if (defined SUPERVISOR_MODE && defined __m68k__) || (defined MACHINE_MODE && defined __riscv)
void put_char(char c)
{
	volatile char *pUart = (char *)UART_BASE;
	pUart[UART_DATA] = c;
}

void put_hex_num(unsigned int n)
{
	for (int count = 7; count >= 0; count--)
	{
		unsigned int val = (n >> (count * 4)) & 0xf;
		if (val < 10)
			put_char('0' + val);
		else
			put_char('a' + val - 10);
	}
}

void put_hex_byte(unsigned char n)
{
	for (int count = 1; count >= 0; count--)
	{
		unsigned int val = (n >> (count * 4)) & 0xf;
		if (val < 10)
			put_char('0' + val);
		else
			put_char('a' + val - 10);
	}
}

void put_dec_short_num(unsigned short i, bool leading)
{
	bool has_printed = leading;
	for (short count = 4; count >= 0; count--)
	{
		unsigned short divide_by = 1;
		for (short inner = 0; inner < count; inner++)
			divide_by *= 10;

		unsigned short div = i / divide_by;
		i = i % divide_by;

		if (div || has_printed)
		{
			has_printed = true;
			put_char(div + '0');
		}
	}
}

void put_string(const char *p)
{
	while (*p)
		put_char(*p++);
}

bool is_usb_connected(void)
{
#ifdef UART_PWREN_BIT
	volatile unsigned char *pUartC = (unsigned char *)UART_BASE;

	unsigned char c = pUartC[UART_STATUS];
	if ((c & (1 << UART_PWREN_BIT)) == 0)
		return true;
	else
		return false;
#else
	return true;
#endif
}

bool is_data_available(void)
{
	volatile unsigned char *pUartC = (unsigned char *)UART_BASE;

	volatile unsigned char c = pUartC[UART_STATUS];

	if (c & (1 << UART_RXF_BIT))
		return false;				//bit set = no data
	else
		return true;
}

//blocking read
unsigned char get_char(void)
{
	volatile unsigned char *pUartC = (unsigned char *)UART_BASE;

	while (!is_data_available())
		;

	return pUartC[UART_DATA];
}
#endif


void put_char_user(char c)
{
	trap1(TRAP_PRINT_CHAR, c);
}

void put_hex_num_user(unsigned int n)
{
	trap1(TRAP_PRINT_HEX_NUM, n);
}

void put_hex_byte_user(unsigned char n)
{
	trap1(TRAP_PRINT_HEX_BYTE, n);
}

void put_dec_num_user(unsigned int i, bool leading)
{
	trap2(TRAP_PRINT_DEC_SHORT_NUM, i, leading ? 1 : 0);
}

void put_string_user(const char *p)
{
	trap1(TRAP_PRINT_STRING, (unsigned int)p);
}
