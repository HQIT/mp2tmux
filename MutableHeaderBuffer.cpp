#include <memory>
#include "MutableHeaderBuffer.h"

using namespace com::cloume::cap;

MutableHeaderBuffer::MutableHeaderBuffer(unsigned int dataCapacity, unsigned int headerCapacity /* = 128 */)
	: BasicBinaryBuffer(headerCapacity + dataCapacity), mDataLength(0)
{
	mpHeaderBegin = mpDataBegin = this->Read() + headerCapacity;
}

MutableHeaderBuffer::~MutableHeaderBuffer(){}

char *MutableHeaderBuffer::HeadedData(){
	return mpHeaderBegin;
}

char *MutableHeaderBuffer::HeadlessData(){
	return mpDataBegin;
}

bool MutableHeaderBuffer::DataFull(){
	return (this->Capacity() - HeaderCapacity()) == mDataLength;
}

unsigned int MutableHeaderBuffer::HeaderLength(){
	return unsigned int(mpDataBegin - mpHeaderBegin);
}

unsigned int MutableHeaderBuffer::HeaderCapacity(){
	return unsigned int(mpDataBegin - this->Read());
}

unsigned int MutableHeaderBuffer::HeadedDataLength(){
	return HeaderLength() + HeadlessDataLength();
}

unsigned int MutableHeaderBuffer::HeadlessDataLength(){
	return mDataLength;
}

unsigned int MutableHeaderBuffer::AppendData(const char *data, unsigned int size){

	unsigned int dataCapacity = this->Capacity() - HeaderCapacity();
	if(dataCapacity - HeadlessDataLength() < size){
		return 0;
	}

	int s = this->Write(data, size, HeaderCapacity() + HeadlessDataLength());
	mDataLength += size;

	return s;
}

unsigned int MutableHeaderBuffer::AppendHeader(const char *header, unsigned int size){

	//头部空间不够放下新的头@header
	if(size + HeaderLength() > HeaderCapacity()){
		return 0;
	}

	mpHeaderBegin -= size;

	return this->Write(header, size, HeaderCapacity() - HeaderLength());
}

void MutableHeaderBuffer::Reset(){
	mpHeaderBegin = mpDataBegin;
	mDataLength = 0;
	
	BasicBinaryBuffer::Reset();
}