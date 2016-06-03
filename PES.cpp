#include "Program.h"
#include "PES.h"

#include <stdint.h>

void PES::Fill(unsigned char* data, ESDataType type)
{
	mType = type;

	memcpy(mData + SIMPLE_HEADER_LENGTH, data, mPayloadLength);

	*mData = 0x00; *(mData + 1) = 0x00; *(mData + 2) = 0x01;	//Packet start code prefix

	//Stream id, only one audio and video, index == 0
	if(type == ESDT_VIDEO){
		*(mData + 3) = 0xE0 + (0 & 0x0F);		//0xE0-0xEF
	}else{	//AUDIO
		*(mData + 3) = 0xC0 + (0 & 0x1F);		//0xC0-0xDF
	}

	//bytes 4-5 is packet length
	unsigned long length = mPayloadLength + 8;
	if(length > 0xFFFF){ 
		length = 0;
	}
	*(mData + 4) = (unsigned char)(length >> 8);
	*(mData + 5) = (unsigned char)(length >> 0);

	//Optional PES header
	*(mData + 6) = 0x80;
	*(mData + 7) = 0x80;		//PTS only

	*(mData + 8) = 5;	//PTS needs 5 bytes
}

void PES::SetPTS(unsigned long long ts){
	uint64_t ipts = (uint64_t)((ts / 10000000.) * 90000);
//	printf("ipts: %d, diff: %f \n", ipts, ts / 10000000.);

#pragma warning(disable: 4244)
#if 1
	// Fill in the PES PTS
	*(mData + 9) = 0x20 | ( ipts >> 29 ) | 0x01;
	*(mData + 10) = ipts >> 22;
	*(mData + 11) = (ipts >> 14) | 0x01;
	*(mData + 12) = ipts >> 7;
	*(mData + 13) = (ipts << 1) | 0x01;
#else
	*(mData + 9) = 0x20 | ( 0 >> 29 ) | 0x01;
	*(mData + 10) = 0 >> 22;
	*(mData + 11) = (0 >> 14) | 0x01;
	*(mData + 12) = 0 >> 7;
	*(mData + 13) = (0 << 1) | 0x01;
#endif
}