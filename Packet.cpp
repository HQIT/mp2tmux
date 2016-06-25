#include "Packet.h"

using namespace com::cloume::cap::streaming;

unsigned long Packet::Fill(unsigned char* data, unsigned long size){
	unsigned long cpSize = this->PayloadCapacity();
	if(size < cpSize){
		cpSize = size;
		if(Header()->AFE() == 0x03){
			//标准规定不足一帧TS包时,使用Adaptation进行填充
			AdaptationFieldLength(this->PayloadCapacity() - cpSize - 1);
		}
	}

	memcpy(this->Payload() + mFilledSize, data, cpSize);
	mFilledSize += cpSize;

	return cpSize;
}

void Packet::Reset(){
	Header()->AFE(0x01);
	Header()->PUSI(0x00);
	
	SetCC(Header()->CC() + 1);

	memset(Body(), 0xFF, PACKET_LENGTH - PACKET_HEADER_LENGTH);

	mFilledSize = 0;
}

void Packet::SetCC(unsigned char cc){
	Header()->CC(cc & 0x0F);
}

void Packet::AFE(unsigned char afe){
	Header()->AFE(afe);
}

unsigned char Packet::AFE(){
	return Header()->AFE();
}