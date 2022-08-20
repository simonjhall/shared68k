#include "MBR.h"
#include "uart.h"
#include "common.h"

const unsigned int MBR::sm_partitionOffsets[4] = {0x1be, 0x1ce, 0x1de, 0x1ee};

MBR::MBR(SdCard &rRoot)
: m_rRoot(rRoot), m_numPartitions(0)
{
	unsigned char head[512];
	bool ok = m_rRoot.ReadBlock(head, 0);
	ASSERT(ok);

	for (unsigned int count = 0; count < 4; count++)
	{
		PTE *partition = (PTE *)&head[sm_partitionOffsets[count]];

		if (partition->m_systemId)
		{
			put_string("partition ");
			put_hex_byte(count);
			put_string(" relative sector ");

			unsigned int relativeSector = partition->m_relativeSectorA | (partition->m_relativeSectorB << 8)
					| (partition->m_relativeSectorC << 16) | (partition->m_relativeSectorD << 24);

			unsigned int totalSectors = partition->m_totalSectorsA | (partition->m_totalSectorsB << 8)
					| (partition->m_totalSectorsC << 16) | (partition->m_totalSectorsD << 24);

			put_hex_num(relativeSector);
			put_string(" total sectors ");
			put_hex_num(totalSectors); put_char('\n');

			m_partitions[m_numPartitions].Init(m_rRoot, relativeSector * 512);
			m_numPartitions++;
		}
	}
}

MBR::~MBR()
{

}

unsigned int MBR::GetNumPartitions(void)
{
	return m_numPartitions;
}

MBR::Partition *MBR::GetPartition(unsigned int p)
{
	if (p < m_numPartitions)
		return &m_partitions[p];
	else
		return 0;
}

