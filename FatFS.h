/*
 * FatFS.h
 *
 *  Created on: 31 May 2013
 *      Author: simon
 */

#ifndef FatFS_H_
#define FatFS_H_

#include "uart.h"
#include "common.h"
#include "MBR.h"

#include <unistd.h>
#include <string.h>

class FatFS;

class FatDirectory
{
public:
	FatDirectory(const char *pName, FatDirectory *pParent, FatFS &fileSystem,
			unsigned int cluster)
	:
	  m_cluster(cluster),
	  m_pParent(pParent),
	  m_rFileSystem(fileSystem)
	{
	};

	bool Fstat(struct stat64 &rBuf);

	FatDirectory *GetParent(void) { return m_pParent; };

	unsigned int GetStartCluster(void)
	{
		return m_cluster;
	}

protected:
	unsigned int m_cluster;

private:
	FatDirectory *m_pParent;
	FatFS &m_rFileSystem;
};

class FatFile
{
public:
	FatFile(const char *pName, FatDirectory *pParent, FatFS &fileSystem,
			unsigned int cluster, unsigned int size)
	:
	  m_cluster(cluster),
	  m_size(size),
	  m_rFileSystem(fileSystem)
	{
	};

	ssize_t ReadFrom(void *pBuf, size_t count, off_t offset);
	ssize_t WriteTo(const void *pBuf, size_t count, off_t offset);
	bool Seekable(off_t);

	bool Fstat(struct stat64 &rBuf);

protected:
	unsigned int m_cluster;
	unsigned int m_size;

private:
	FatFS &m_rFileSystem;
};

class FatFS
{
	friend class FatFile;
	typedef MBR::Partition BaseDevice;

public:
	static const unsigned int sm_allocation = 1024;
	FatFS(BaseDevice &rDevice, char *pAllocation);
	~FatFS();

	// BaseDirent *OpenByHandle(BaseDirent &rFile, unsigned int flags);
	// bool Close(BaseDirent &);

	bool Stat(const char *pFilename, struct stat &rBuf);
	bool Lstat(const char *pFilename, struct stat &rBuf);

	bool Mkdir(const char *pFilePath, const char *pFilename);
	bool Rmdir(const char *pFilename);

// protected:
	FatDirectory &GetRootDirectory(void);

	struct FATDirEnt;

	//init
	void ReadBpb(void);
	void ReadEbr(void);
	void HaveFatMemory(void *pFat);
	void LoadFat(void);

	//data reading
	void ReadCluster(void *pDest, unsigned int cluster, unsigned int sizeInBytes);
	unsigned int CountClusters(unsigned int start);
	void ReadClusterChain(void *pCluster, unsigned int cluster);

	//directory walking
	void BuildDirectoryStructure(void);
	void ListDirectory(FatDirectory &rDir);
	bool IterateDirectory(void *pCluster, unsigned int &rEntry, FATDirEnt &rOut, bool reentry = false);

	//translation
	inline unsigned int SectorToBytes(unsigned int b) const
	{
		return b * m_biosParameterBlock.m_bytesPerSector;
	}

	inline unsigned int ClusterToSector(unsigned int c) const
	{
		return c * m_biosParameterBlock.m_sectorsPerCluster;
	}

	inline unsigned int RelativeToAbsoluteCluster(unsigned int c)
	{
		return c - 2 + RootDirectoryAbsCluster();
	}

	//accessors

	inline unsigned int ClusterSizeInBytes(void) const
	{
		return SectorToBytes(ClusterToSector(1));
	}

	unsigned int FatSize(void) const
	{
		if (m_fatVersion == 32)
			return SectorToBytes(m_extendedBootRecord32.m_sectorsPerFat);
		else
			return SectorToBytes(m_biosParameterBlock.m_sectorsPerFat);
	}

	inline unsigned int RootDirectoryAbsCluster(void) const
	{
		if (m_fatVersion == 32)
			return m_biosParameterBlock.m_reservedSectors + m_biosParameterBlock.m_numFats * m_extendedBootRecord32.m_sectorsPerFat;		//div sectors/cluster
		else
			return m_biosParameterBlock.m_reservedSectors + m_biosParameterBlock.m_numFats * m_biosParameterBlock.m_sectorsPerFat;		//div sectors/cluster
	}

	inline unsigned int RootDirectoryRelCluster(void) const
	{
		return 2;
	}

	//walking clusters
	bool NextCluster(unsigned int current, unsigned int &rNext)
	{
		switch (m_fatVersion)
		{
		case 32:
			return NextCluster32(current, rNext);
		case 16:
			return NextCluster16(current, rNext);
		case 12:
			return NextCluster12(current, rNext);
		default:
			return false;
		}
	}

	bool NextCluster32(unsigned int current, unsigned int &rNext) const;
	bool NextCluster16(unsigned int current, unsigned int &rNext) const;
	bool NextCluster12(unsigned int current, unsigned int &rNext) const;

	BaseDevice &m_rDevice;
	int m_fatVersion;
	unsigned char *m_pFat;

	struct FATDirEnt
	{
		char m_name[255];
		unsigned int m_cluster;
		unsigned int m_size;

		enum FATDirEntType
		{
			kReadOnly = 1,
			kHidden = 2,
			kSystem = 4,
			kVolumeId = 8,
			kDirectory = 16,
			kArchive = 32,
		} m_type;
	};

#pragma pack(push)
#pragma pack(1)
	struct Bpb
	{
		unsigned char m_jmpShort3cNop[3];
		char m_oemIdentifier[8];
		unsigned short m_bytesPerSector;
		unsigned char m_sectorsPerCluster;
		unsigned short m_reservedSectors;
		unsigned char m_numFats;
		unsigned short m_numDirectoryEntries;
		unsigned short m_totalSectors;
		unsigned char m_mediaDescriptorType;
		unsigned short m_sectorsPerFat;
		unsigned short m_sectorsPerTrack;
		unsigned short m_numHeads;
		unsigned int m_hiddenSectors;
		unsigned int m_sectorsOnMedia;
	} m_biosParameterBlock;

	struct Ebr1216
	{
		unsigned char m_driveNumber;
		unsigned char m_flags;
		unsigned char m_signature;
		unsigned int m_volumeId;
		char m_volumeLabel[11];
		char m_systemIdentifier[8];
	};
	struct Ebr32
	{
		unsigned int m_sectorsPerFat;
		unsigned short m_flags;
		unsigned short m_fatVersion;
		unsigned int m_rootDirectoryCluster;
		unsigned short m_fsinfoCluster;
		unsigned short m_backupBootSectorCluster;
		unsigned char m_reserved[12];
		unsigned char m_driveNumber;
		unsigned char m_moreFlags;
		unsigned char m_signature;
		unsigned int m_volumeId;
		char m_volumeLabel[11];
		char m_systemIdentifier[8];
	};

	union {
		Ebr1216 m_extendedBootRecord1216;
		Ebr32 m_extendedBootRecord32;
	};

	struct DirEnt83
	{
		char m_name83[11];
		unsigned char m_attributes;
		unsigned char m_reserved;
		unsigned char m_createTimeTenths;
		unsigned short m_createTime;
		unsigned short m_createDate;
		unsigned short m_lastAccessedDate;
		unsigned short m_highCluster;
		unsigned short m_lastModifiedTime;
		unsigned short m_lastModifiedDate;
		unsigned short m_lowCluster;
		unsigned int m_fileSize;
	};

	struct DirEntLong
	{
		unsigned char m_order;
		unsigned short m_firstFive[5];
		unsigned char m_attribute;
		unsigned char m_type;
		unsigned char m_checksum;
		unsigned short m_nextSix[6];
		unsigned char m_zero[2];
		unsigned short m_finalTwo[2];
	};

#pragma pack(pop)

private:
	FatDirectory *m_pRoot;
};

#endif /* FatFS_H_ */
