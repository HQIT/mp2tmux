#include "Program.h"
#include "PES.h"

using namespace com::cloume::cap::streaming;

PES::PES(char *data, unsigned int size, unsigned int timestamp)
	: BasicBinaryBuffer(size), mTimestamp(timestamp)
{
	Write(data, size);
	Length(size);
}

unsigned int PES::Timestamp(){
	return mTimestamp;
}

///@timestamp
PESHeader::PESHeader(ESDataType type, unsigned int payloadLength, unsigned int timestamp /* = 0 */)
	: BasicBinaryBuffer(SIMPLE_PES_HEADER_LENGTH)
{
	char *pData = this->Read();

	//Packet start code prefix
	*pData = 0x00; *(pData + 1) = 0x00; *(pData + 2) = 0x01;

#pragma warning (disable: 4309)
	//Stream id, only one audio and video, index == 0
	if(type == ESDT_VIDEO){
		*(pData + 3) = 0xE0 + (0 & 0x0F);		//0xE0-0xEF
	}else{	//AUDIO
		*(pData + 3) = 0xC0 + (0 & 0x1F);		//0xC0-0xDF
	}

	//bytes 4-5 is packet length
	unsigned long length = payloadLength + 8;
	if(length > 0xFFFF){ 
		length = 0;
	}
	*(pData + 4) = (unsigned char)(length >> 8);
	*(pData + 5) = (unsigned char)(length >> 0);

	//Optional PES header
	*(pData + 6) = 0x80;
	*(pData + 7) = 0x80;		//PTS only
#pragma warning(default: 4309)

	*(pData + 8) = 5;	//PTS needs 5 bytes
	unsigned long ipts = timestamp * 90;
#pragma warning(disable: 4244)
	// Fill in the PES PTS
	*(pData + 9) = 0x20 | ( ipts >> 29 ) | 0x01;
	*(pData + 10) = ipts >> 22;
	*(pData + 11) = (ipts >> 14) | 0x01;
	*(pData + 12) = ipts >> 7;
	*(pData + 13) = (ipts << 1) | 0x01;
#pragma warning(default: 4244)
}

unsigned int PESHeader::Length(){
	return Capacity();
}