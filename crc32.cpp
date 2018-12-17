/*
 * crc32.cpp
 *
 *  Created on: 29 Aug 2018
 *      Author: simon
 */

//http://www.hackersdelight.org/hdcodetxt/crc.c.txt
unsigned int crc32b(unsigned char *message, int len)
{
	int i, j;
	unsigned int byte, crc, mask;

	crc = 0xFFFFFFFF;
	for (i = 0; i < len; i++)
	{
		byte = message[i];            // Get next byte.
		crc = crc ^ byte;
		for (j = 7; j >= 0; j--) {    // Do eight times.
			mask = -(crc & 1);
			crc = (crc >> 1) ^ (0xEDB88320 & mask);
		}
	}
	return ~crc;
}


