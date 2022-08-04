#include "uart.h"
#include "common.h"
#include "spi.h"

#ifdef HAS_SPI
void spi_set_clock_cs(unsigned int clock_divider, bool selected)
{
	ASSERT(clock_divider < 128);
	volatile unsigned char *pAddr = (volatile unsigned char *)SPI_BASE + SPI_CLOCK_CS;
	*pAddr = clock_divider | (selected ? 0x80 : 0);
}

unsigned char spi_send_receive(unsigned char b)
{
	volatile unsigned char *pAddr = (volatile unsigned char *)SPI_BASE + SPI_DATA;
	*pAddr = b;
	return *pAddr;
}
#endif
