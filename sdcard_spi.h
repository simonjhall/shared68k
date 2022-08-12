#ifndef _SDCARD_SPI_H_
#define _SDCARD_SPI_H_

#include "spi.h"

class SdCard
{
public:
	enum Error
	{
		kErrorNoError,
		kErrorInitTimeout,
		kErrorCmd8Error,
		kErrorNotSdCard,
	};

	Error Init(void);
	bool ReadBlock(char *pBuffer, unsigned int block);

	static const unsigned int kBlockSize = 512;

private:
	static const bool kWithCrcSupport = false;

	//clocks
	static const unsigned int kSocSpeedHz	= 50000000;
	static const unsigned int kInitSpeedHz	= 250000;
	static const unsigned int kFullSpeedHz	= 4000000;

	//time-outs
	// static const unsigned int kInitTimeout = kSocSpeedHz / 10;
	// static const unsigned int kReadTimeout = kSocSpeedHz / 10;
	// static const unsigned int kIdleTimeout = kSocSpeedHz / 10;
	static const unsigned int kInitTimeout = kSocSpeedHz * 2;
	static const unsigned int kReadTimeout = kSocSpeedHz / 3;
	static const unsigned int kIdleTimeout = kSocSpeedHz / 3;

	//responses
	//R1
	static const unsigned int kR1InIdleState		= 1 << 0;
	static const unsigned int kR1EraseReset			= 1 << 1;
	static const unsigned int kR1IllegalCommand		= 1 << 2;
	static const unsigned int kR1ComCrcError		= 1 << 3;
	static const unsigned int kR1EraseSequenceError	= 1 << 4;
	static const unsigned int kR1AddressError		= 1 << 5;
	static const unsigned int kR1ParameterError		= 1 << 6;
	//R3 - R1 response followed by four byte OCR register
	//R7 - R1 response followed by extra four bytes

	enum CardType
	{
		kCardTypeStandardCapacityV1,
		kCardTypeStandardCapacityV2,
		kCardTypeHighCapacityV2,
	};

	enum Cmd
	{
		kCmd0 = 0,			//GO_IDLE_STATE
		kCmd8 = 8,			//SEND_IF_COND, R7 response
		kCmd17 = 17,		//READ_SINGLE_BLOCK
		kCmd55 = 55,		//APP_CMD
		kCmd58 = 58,		//READ_OCR, R3 response
	};

	enum Acmd
	{
		kAcmd41 = 41,		//SD_SEND_OP_COND
	};

	unsigned char SendCommand(unsigned int command, unsigned int arg);
	unsigned char SendAppCommand(unsigned int command, unsigned int arg);

	static unsigned int HzToClockDivider(unsigned int hz);

	unsigned int m_maxClockDivider;
	unsigned int m_currentClockDivider;
	bool m_selected;

	CardType m_type;
};

#endif
