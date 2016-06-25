#include "PSI.h"
#include "PMT.h"
#include "Program.h"

using namespace com::cloume::cap::streaming;

unsigned short PMT::PCRPID(){
	return mProgram->PCRPID();
}

unsigned short PMT::PID(){
	return mProgram->PMTPID();
}

unsigned short PMT::UserDefinedData(){
	return mProgram->Number();
}

unsigned short PMT::SectionLength(){
	return unsigned short(
		5 
		+ 4		//NO descriptor
		+ mProgram->Streams().size() * 5	//each stream need 5 bytes (NO descriptor)
		+ 4);	//CRC32
}

unsigned short PMT::ProgramInfoLength(){
	return 0;
}

unsigned long PMT::BitstreamSize(){
	return unsigned long(
		9		//TABLE info
		+ 2		//PCR PID
		+ 2 + ProgramInfoLength()
		+ 5 * mProgram->Streams().size()	//Elementary list, NO ES Descriptor
		+ 4);	//CRC32
}

int PMT::BitstreamAfterTableInfo(unsigned char* pBuffer){

	*pBuffer = (0x07 << 5) | (PCRPID() >> 8);
	*(pBuffer + 1) = PCRPID() >> 0;

	*(pBuffer + 2) = 0xF0;	//Reserved
	*(pBuffer + 2) &= 0xFC;
	*(pBuffer + 2) |= (ProgramInfoLength() >> 8);
	*(pBuffer + 3) = (ProgramInfoLength() >> 0);

	//Elementary Stream List
	Program::StreamVector streams = mProgram->Streams();
	Program::StreamVector::iterator it = streams.begin();

	pBuffer = pBuffer + 4;
	while(it != streams.end()){
		Program::Stream *s = *it;

		*pBuffer = s->SubType();
		*(pBuffer + 1) = (0x07 << 5) | (s->ElementaryPID() >> 8);
		*(pBuffer + 2) = s->ElementaryPID() >> 0;

		//ES Info Length : 0
		*(pBuffer + 3) = 0xF0;
		*(pBuffer + 4) = 0x00;

		pBuffer = pBuffer + 5;

		++it;
	}

	return 0;
}