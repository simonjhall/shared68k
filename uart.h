/*
 * uart.h
 *
 *  Created on: 29 Aug 2018
 *      Author: simon
 */

#ifndef UART_H_
#define UART_H_

#ifdef __cplusplus
extern "C" {
#endif

#if (defined SUPERVISOR_MODE && defined __m68k__) || (defined MACHINE_MODE && defined __riscv)
void put_char(char c);
void put_hex_num(unsigned int n);
void put_hex_byte(unsigned char n);
void put_dec_short_num(unsigned short i, bool leading);
void put_string(const char *p);

inline void put_string_n(const char *p, unsigned int n)
{
	for (unsigned int count = 0; count < n; count++)
		put_char(p[count]);
}

bool is_usb_connected(void);
bool is_data_available(void);
bool is_space_available(void);

unsigned char get_char(void);
#endif

void put_char_user(char c);
void put_hex_num_user(unsigned int n);
void put_hex_byte_user(unsigned char n);
void put_dec_num_user(unsigned int i, bool leading);
void put_string_user(const char *p);

inline void put_string_n_user(const char *p, unsigned int n)
{
	for (unsigned int count = 0; count < n; count++)
		put_char_user(p[count]);
}

#ifdef __cplusplus
}
#endif


#endif /* UART_H_ */
