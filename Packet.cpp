#include "Packet.h"

unsigned long Packet::Fill(unsigned char* data, unsigned long size){
	unsigned long cpSize = this->PayloadCapacity();
	if(size < cpSize){
		cpSize = size;
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