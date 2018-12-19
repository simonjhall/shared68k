/*
 * common.h
 *
 *  Created on: 29 Aug 2018
 *      Author: simon
 */

#ifndef COMMON_H_
#define COMMON_H_

/* big ham origins */
#if 0
#define ROM_BASE	((void *)(0))
#define ROM_SIZE	(32 * 1024)

#define UART_BASE	((void *)((1 << 21) | (1 << 20) | (1 << 19)))
#define UART_DATA	(0)
#define UART_STATUS	(1)

#define UART_RXF_BIT	(0)
#define UART_TXE_BIT	(1)
#define UART_PWREN_BIT	(2)

#define RAM_BASE	((void *)(1 << 20))
#define RAM_SIZE	(2 * 1024 * 1024)

#define LOAD_POINT	((void *)((1 << 20) | 0x100))
#endif

/* big daddy pro */
#if 0
#define ROM_BASE	((void *)(0))
#define ROM_SIZE	(32 * 1024)

#define UART_BASE	((void *)(0b001 << 21))
#define UART_DATA	(0)
#define UART_STATUS	(1)

#define UART_RXF_BIT	(0)
#define UART_TXE_BIT	(1)
#define UART_PWREN_BIT	(2)

#define RAM_BASE	((void *)(0b010 << 21))
#define RAM_SIZE	(1 * 1024 * 1024)

#define HAS_PCI
#define PCI_IO		(0b100 << 21)
#define PCI_MEM		(0b101 << 21)
#define PCI_CONFIG	(0b110 << 21)

#define PCI_IO_PASSTHROUGH(a20a19) ((a20a19) << 19)

#define LOAD_POINT	((void *)((0b010 << 21) | 0x100))


#endif

/* dynamic daddy /*/
#if 1
#define ROM_BASE	((void *)(0))
#define ROM_SIZE	(32 * 1024)

#define UART_BASE	((void *)(1 << 15))
#define UART_DATA	(0)
#define UART_STATUS	(2)

#define UART_RXF_BIT	(2)
#define UART_TXE_BIT	(3)
#define UART_PWREN_BIT	(4)

#define HAS_I2C
#define I2C_BASE		((unsigned char *)((1 << 15) | 2))
#define I2C_SCLK_BIT	(0)
#define I2C_SDA_BIT		(1)

#define RAM_BASE	((void *)(1 << 23))
#define RAM_SIZE 	(4 * 1024 * 1024)

#define LOAD_POINT	((void *)((1 << 23) | 0x100))
#endif


#define ASSERT(x) \
		if (!(x))\
		{\
			put_string("assert ");\
			put_char(' ');\
			put_string(__FUNCTION__);\
			put_char(' ');\
			put_string(__FILE__);\
			put_char(' ');\
			put_dec_short_num(__LINE__, true);\
			while (1);\
		}



#endif /* COMMON_H_ */
