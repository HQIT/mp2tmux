#include "PSI.h"
#include "CRC.h"

#include <memory>

using namespace com::cloume::common;
using namespace com::cloume::cap::streaming;

PSI::PSI() : mBitstream(0){
}

void PSI::UpdateBitstream(){
	mBitstream.resize(BitstreamSize(), 0);

	unsigned char *pBuffer = (unsigned char *)mBitstream.data();
	int size = BitstreamSize();

	//*pBuffer = 0;
	*(pBuffer + 1) = TableId();
	
	*(pBuffer + 2) 
		= 0x01 << 7		//SectionSyntaxIndicator()
		| 0
		| 0x03 << 4
		| 0x00;

	//Section Length
	*(pBuffer + 2) &= 0xFC;		//clear 2 bits
	*(pBuffer + 2) |= ((SectionLength() >> 8) & 0x03);
	*(pBuffer + 3) = (unsigned char)SectionLength() & 0xFF;

	//User defined data. PAT:transport stream ID; PMT:Program num;
	*(pBuffer + 4) = UserDefinedData() >> 8;
	*(pBuffer + 5) = UserDefinedData() >> 0;
	
	*(pBuffer + 6)
		= 0x03 << 6		//Reserved
		| 0				//Version, never change
		| 0x01;

	*(pBuffer + 7) = SectionNumber();
	*(pBuffer + 8) = LastSectionNumber();

	BitstreamAfterTableInfo(pBuffer + 9);

	//CRC32
	unsigned long crc = CRC32::CaculateCRC(pBuffer + 1, size - 4 - 1);
	unsigned int pos = size - 4;
#pragma warning(disable: 4244)
	*(pBuffer + pos) = crc >> 24;
	*(pBuffer + pos + 1) = crc >> 16;
	*(pBuffer + pos + 2) = crc >> 8;
	*(pBuffer + pos + 3) = crc >> 0;
#pragma warning(default: 4244)
}

char *PSI::Bitstream(){
	return mBitstream.data();
}