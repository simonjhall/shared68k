#include "uart.h"
#include "common.h"
#include "sdcard_spi.h"

// #define GENERATE

// http://elm-chan.org/docs/mmc/mmc_e.html
// http://users.ece.utexas.edu/~valvano/EE345M/SD_Physical_Layer_Spec.pdf
// https://electronics.stackexchange.com/questions/321304/how-to-use-the-data-crc-of-sd-cards-in-spi-mode

#ifdef GENERATE
#include <stdio.h>

/* CRC7 table (for SD commands) */
static unsigned char sd_crc7_table[256];

/* CRC16 table (for SD data) */
static unsigned short sd_crc16_table[256];

static void generate_crc7_table(void)
{
    unsigned int crc;
    unsigned int bit;
    unsigned int byt;

    /* Generate CRC7 table */
    for (byt = 0U; byt < 256U; byt ++)
    {
        crc = byt;
        if ((crc & 0x80U) != 0U)
        {
            crc ^= 0x89U;
        }
        for (bit = 1U; bit < 8U; bit ++)
        {
            crc <<= 1;
            if ((crc & 0x80U) != 0U)
            {
                crc ^= 0x89U;
            }
        }
        sd_crc7_table[byt] = (crc & 0x7FU);
    }
}

static void generate_crc16_table(void)
{
    unsigned int crc;
    unsigned int bit;
    unsigned int byt;

    /* Generate CRC16 table */
    for (byt = 0U; byt < 256U; byt ++)
    {
        crc = byt << 8;
        for (bit = 0U; bit < 8U; bit ++)
        {
            crc <<= 1;
            if ((crc & 0x10000U) != 0U)
            {
                crc ^= 0x1021U;
            }
        }
        sd_crc16_table[byt] = (crc & 0xFFFFU);
    }
}

int main(void)
{
    generate_crc7_table();
    generate_crc16_table();

    printf("static unsigned char sd_crc7_table[256] = {\n");
    for (int i = 0; i < 16; i++)
    {
        printf("\t");
        for (int j = 0; j < 16; j++)
        {
            printf("0x%02x, ", sd_crc7_table[i * 16 + j]);
        }
        printf("\n");
    }
    printf("};\n");

    printf("static unsigned short sd_crc16_table[256] = {\n");
    for (int i = 0; i < 16; i++)
    {
        printf("\t");
        for (int j = 0; j < 16; j++)
        {
            printf("0x%04x, ", sd_crc16_table[i * 16 + j]);
        }
        printf("\n");
    }
    printf("};\n");
}

#else
static unsigned char sd_crc7_table[256] = {
	0x00, 0x09, 0x12, 0x1b, 0x24, 0x2d, 0x36, 0x3f, 0x48, 0x41, 0x5a, 0x53, 0x6c, 0x65, 0x7e, 0x77, 
	0x19, 0x10, 0x0b, 0x02, 0x3d, 0x34, 0x2f, 0x26, 0x51, 0x58, 0x43, 0x4a, 0x75, 0x7c, 0x67, 0x6e, 
	0x32, 0x3b, 0x20, 0x29, 0x16, 0x1f, 0x04, 0x0d, 0x7a, 0x73, 0x68, 0x61, 0x5e, 0x57, 0x4c, 0x45, 
	0x2b, 0x22, 0x39, 0x30, 0x0f, 0x06, 0x1d, 0x14, 0x63, 0x6a, 0x71, 0x78, 0x47, 0x4e, 0x55, 0x5c, 
	0x64, 0x6d, 0x76, 0x7f, 0x40, 0x49, 0x52, 0x5b, 0x2c, 0x25, 0x3e, 0x37, 0x08, 0x01, 0x1a, 0x13, 
	0x7d, 0x74, 0x6f, 0x66, 0x59, 0x50, 0x4b, 0x42, 0x35, 0x3c, 0x27, 0x2e, 0x11, 0x18, 0x03, 0x0a, 
	0x56, 0x5f, 0x44, 0x4d, 0x72, 0x7b, 0x60, 0x69, 0x1e, 0x17, 0x0c, 0x05, 0x3a, 0x33, 0x28, 0x21, 
	0x4f, 0x46, 0x5d, 0x54, 0x6b, 0x62, 0x79, 0x70, 0x07, 0x0e, 0x15, 0x1c, 0x23, 0x2a, 0x31, 0x38, 
	0x41, 0x48, 0x53, 0x5a, 0x65, 0x6c, 0x77, 0x7e, 0x09, 0x00, 0x1b, 0x12, 0x2d, 0x24, 0x3f, 0x36, 
	0x58, 0x51, 0x4a, 0x43, 0x7c, 0x75, 0x6e, 0x67, 0x10, 0x19, 0x02, 0x0b, 0x34, 0x3d, 0x26, 0x2f, 
	0x73, 0x7a, 0x61, 0x68, 0x57, 0x5e, 0x45, 0x4c, 0x3b, 0x32, 0x29, 0x20, 0x1f, 0x16, 0x0d, 0x04, 
	0x6a, 0x63, 0x78, 0x71, 0x4e, 0x47, 0x5c, 0x55, 0x22, 0x2b, 0x30, 0x39, 0x06, 0x0f, 0x14, 0x1d, 
	0x25, 0x2c, 0x37, 0x3e, 0x01, 0x08, 0x13, 0x1a, 0x6d, 0x64, 0x7f, 0x76, 0x49, 0x40, 0x5b, 0x52, 
	0x3c, 0x35, 0x2e, 0x27, 0x18, 0x11, 0x0a, 0x03, 0x74, 0x7d, 0x66, 0x6f, 0x50, 0x59, 0x42, 0x4b, 
	0x17, 0x1e, 0x05, 0x0c, 0x33, 0x3a, 0x21, 0x28, 0x5f, 0x56, 0x4d, 0x44, 0x7b, 0x72, 0x69, 0x60, 
	0x0e, 0x07, 0x1c, 0x15, 0x2a, 0x23, 0x38, 0x31, 0x46, 0x4f, 0x54, 0x5d, 0x62, 0x6b, 0x70, 0x79, 
};
static unsigned short sd_crc16_table[256] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 
	0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6, 0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de, 
	0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d, 
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 
	0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823, 0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b, 
	0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a, 
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49, 
	0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70, 0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78, 
	0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067, 
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256, 
	0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d, 0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 
	0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634, 
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3, 
	0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92, 
	0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1, 
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0, 
};
#endif

/* Running CRC7 calculation for a byte. */
static unsigned char sd_crc7_byte(unsigned int crcval, unsigned int byte)
{
	return sd_crc7_table[(byte ^ (crcval << 1)) & 0xFFU];
}

/* Running CRC16 calculation for a byte. */
static unsigned short sd_crc16_byte(unsigned int crcval, unsigned int byte)
{
	return (sd_crc16_table[(byte ^ (crcval >> 8)) & 0xFFU] ^ (crcval << 8)) & 0xFFFFU;
}

SdCard::Error SdCard::Init(void)
{
	m_maxClockDivider = HzToClockDivider(kFullSpeedHz);
	m_currentClockDivider = HzToClockDivider(kInitSpeedHz);
	m_selected = false;

	//deselect the card
	spi_set_clock_cs(m_currentClockDivider, m_selected);

	// The host shall supply power to the card so that the voltage is reached to Vdd_min within 250ms and
	// start to supply at least 74 SD clocks to the SD card with keeping CMD line to high. In case of SPI
	// mode, CS shall be held to high during 74 clock cycles.

	for (int count = 0; count < 10; count++)
		spi_send_receive(0xff);

	//select the card
	m_selected = true;
	spi_set_clock_cs(m_currentClockDivider, m_selected);
	
	// The SD Card is powered up in the SD mode. It will enter SPI mode if the CS signal is asserted
	// (negative) during the reception of the reset command (CMD0)

	unsigned int start_time = *CYCLE_COUNTER;

	while (SendCommand(kCmd0, 0) != kR1InIdleState)
	{
		unsigned int current_time = *CYCLE_COUNTER;
		if (current_time - start_time > kInitTimeout)
		{
			return kErrorInitTimeout;
		}
	}

	// SEND_IF_COND (CMD8) is used to verify SD Memory Card interface operating condition
	// If the card indicates an illegal command, the card is legacy and does not support CMD8

	//aa = check pattern that is echoed back
	//1 = 2.7-3.6V
	//It is recommended to use ‘10101010b’ for the ‘check pattern’. 
	bool cmd8_supported = true;

	if (SendCommand(kCmd8, 0x1aa) & kR1IllegalCommand)
	{
		//v1 card or not SD card
		cmd8_supported = false;
	}
	else
	{
		//v2 or later sd card
		//read back R7 response
		spi_send_receive(0xff);
		spi_send_receive(0xff);
		spi_send_receive(0xff);
		unsigned char check = spi_send_receive(0xff);
		if (check != 0xaa)
			return kErrorCmd8Error;
	}
	
	start_time = *CYCLE_COUNTER;
	// SD_SEND_OP_COND (ACMD41) is used to start initialization and to check if the card has completed
	// initialization. It is mandatory to issue CMD8 prior to the first ACMD41.
	while (SendAppCommand(kAcmd41, cmd8_supported ? (1 << 30) : 0) != 0)			//HCS set
	{
		// Card returns 'in_idle_state'=1

		//wait for card to become ready
		//could also check for illegal command -> not SD card

		unsigned int current_time = *CYCLE_COUNTER;
		if (current_time - start_time > kInitTimeout)
		{
			return kErrorInitTimeout;
		}
	}

	// After initialization is completed, the host should get CCS information in the response of CMD58. CCS is
	// valid when the card accepted CMD8 and after the completion of initialization. CCS=1 means that the
	// card is a High Capacity SD Memory Card. CCS=0 means that the card is a Standard Capacity SD
	// Memory Card. 
	if (SendCommand(kCmd58, 0) & kR1IllegalCommand)
	{
		return kErrorNotSdCard;
	}

	unsigned char ccs = spi_send_receive(0xff);
	//discard the rest of the OCR
	spi_send_receive(0xff);
	spi_send_receive(0xff);
	spi_send_receive(0xff);
	
	// Bit 31 - Card power up status bit, this status bit is set if the card power up procedure has been finished.
	// Bit 30 - Card capacity status bit, this status bit is set to 1 if card is High Capacity SD Memory Card. 0
	// indicates that the card is Standard Capacity SD Memory Card. The Card Capacity status bit is valid after
	// the card power up procedure is completed and the card power up status bit is set to 1. The Host shall
	// read this status bit to identify a Standard or High Capacity SD Memory Card.

	if (cmd8_supported == false)
		m_type = kCardTypeStandardCapacityV1;
	else if ((ccs & 0xc0) == 0xc0)
		m_type = kCardTypeHighCapacityV2;
	else
		m_type = kCardTypeStandardCapacityV2;
	
	//deselect the card
	m_selected = false;
	spi_set_clock_cs(m_currentClockDivider, m_selected);

	//and change the speed up
	m_currentClockDivider = m_maxClockDivider;
	
	return kErrorNoError;
}

unsigned char SdCard::SendCommand(unsigned int command, unsigned int arg)
{
	// The card is ready to receive a command frame when it drives DO high
	unsigned int start_time = *CYCLE_COUNTER;

	while (spi_send_receive(0xff) != 0xff)
	{
		unsigned int current_time = *CYCLE_COUNTER;
		if (current_time - start_time > kIdleTimeout)
		{
			//todo not sure what to do here to propagate error
			return kR1IllegalCommand;
		}
	}

	//command
	spi_send_receive(command | (1 << 6));		//transmission bit
	//argument
	spi_send_receive((arg >> 24) & 0xff);
	spi_send_receive((arg >> 16) & 0xff);
	spi_send_receive((arg >>  8) & 0xff);
	spi_send_receive((arg >>  0) & 0xff);

	unsigned char crc = 1;		//set end bit

	if (kWithCrcSupport)
	{
		unsigned int crcval = 0x00U;
		crcval = sd_crc7_byte(crcval, command | (1 << 6));
		crcval = sd_crc7_byte(crcval, (arg >> 24) & 0xff);
		crcval = sd_crc7_byte(crcval, (arg >> 16) & 0xff);
		crcval = sd_crc7_byte(crcval, (arg >>  8) & 0xff);
		crcval = sd_crc7_byte(crcval, (arg >>  0) & 0xff);

		crc = (crcval << 1) | 0x01U;
	}
	else
	{
		if (command == kCmd0)		//argument needs to be zero
			crc = 0x95;
		else if (command == kCmd8)	//argument needs to be 0x1aa
			crc = 0x87;
	}

	spi_send_receive(crc);

	start_time = *CYCLE_COUNTER;
	unsigned char response;			//top bit needs to be clear
	while ((response = spi_send_receive(0xff)) & 0x80)
	{
		unsigned int current_time = *CYCLE_COUNTER;
		if (current_time - start_time > kIdleTimeout)
		{
			//todo not sure what to do here to propagate error
			return kR1IllegalCommand;
		}
	}

	return response;
}

unsigned char SdCard::SendAppCommand(unsigned int command, unsigned int arg)
{
	SendCommand(kCmd55, 0);
	return SendCommand(command, arg);
}

unsigned int SdCard::HzToClockDivider(unsigned int hz)
{
	int div = (kSocSpeedHz >> 1) / hz - 1;
	if (div < 0)
		return 0;
	else if (div > 127)
		return 127;
	else
		return div;
}

bool SdCard::ReadBlock(unsigned char *pBuffer, unsigned int block)
{
	//select the card
	m_selected = true;
	spi_set_clock_cs(m_currentClockDivider, m_selected);

	unsigned char ret;
	if (m_type == kCardTypeHighCapacityV2)
		ret = SendCommand(kCmd17, block);
	else
		ret = SendCommand(kCmd17, block << 9);
	
	//needs to be "ready"
	if (ret)
	{
		//deselect the card
		m_selected = false;
		spi_set_clock_cs(m_currentClockDivider, m_selected);
		
		return false;
	}
	
	unsigned int start_time = *CYCLE_COUNTER;

	//wait for the start token
	while (spi_send_receive(0xff) != 0xfe)
	{
		unsigned int current_time = *CYCLE_COUNTER;
		if (current_time - start_time > kReadTimeout)
		{
			//deselect the card
			m_selected = false;
			spi_set_clock_cs(m_currentClockDivider, m_selected);
			
			return false;
		}
	}

	//receive the data
	for (int count = 0; count < 512; count++)
	{
		pBuffer[count] = spi_send_receive(0xff);
	}

	//ignore the crc
	spi_send_receive(0xff);
	spi_send_receive(0xff);

	//deselect the card
	m_selected = false;
	spi_set_clock_cs(m_currentClockDivider, m_selected);

	return true;
}
