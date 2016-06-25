#include "BasicBinaryBuffer.h"

using namespace com::cloume::common;

BasicBinaryBuffer::BasicBinaryBuffer(unsigned int capacity)
	: mBuffer(capacity, 0), mLength(0){
}

BasicBinaryBuffer::~BasicBinaryBuffer(){}

int BasicBinaryBuffer::Write(const char *from, unsigned int fromSize, unsigned int off/* = 0 */){
	if(Capacity() < off){
		return 0;
	}

	int ret = fromSize > Capacity() - off ? Capacity() - off : fromSize;
	if(ret > 0){
		memcpy(mBuffer.data() + off, from, ret);
	}
	
	return ret;
}

void BasicBinaryBuffer::Length(unsigned int len){
	mLength = len;
}

char *BasicBinaryBuffer::Read(unsigned int off, unsigned int &count){
	if(off >= Capacity()){
		count = 0;
		return NULL;
	}

	count = Capacity() - off > count ? count : Capacity() - off;

	return mBuffer.data() + off;
}

char *BasicBinaryBuffer::Read(unsigned int off/* = 0*/){
	unsigned int count = Capacity() - off;
	return Read(off, count);
}

char *BasicBinaryBuffer::Read(){
	return Read(0);
}

char *BasicBinaryBuffer::End(){
	return &(*mBuffer.end());
}

bool BasicBinaryBuffer::Full(){
	return this->Length() == mBuffer.capacity();
}

bool BasicBinaryBuffer::Empty(){
	return this->Length() == 0;
}

unsigned int BasicBinaryBuffer::Length() const{
	return mLength;
}

unsigned int BasicBinaryBuffer::Capacity(){
	return mBuffer.capacity();
}

void BasicBinaryBuffer::Reset(){
	mBuffer.clear();
	mLength = 0;
}