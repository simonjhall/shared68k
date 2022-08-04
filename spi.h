#ifndef _SPI_H_
#define _SPI_H_

void spi_set_clock_cs(unsigned int clock_divider, bool selected);
unsigned char spi_send_receive(unsigned char b);

#endif
