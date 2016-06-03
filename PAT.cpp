#include "PSI.h"
#include "PAT.h"

unsigned long PAT::BitstreamSize(){
	return 
		9		//TABLE info
		+ 4 * mProgramRecords.size()	//PMTs' list
		+ 4;	//CRC32
}

int PAT::BitstreamAfterTableInfo(unsigned char* pBuffer){

	ProgramRecordVector::iterator it = mProgramRecords.begin();
	while(it != mProgramRecords.end()){
		ProgramRecord* pr = *it;
		
		*pBuffer = (pr->ProgramNumber) >> 8;
		*(pBuffer + 1) = (pr->ProgramNumber) >> 0;

		//Reserved
		*(pBuffer + 2) = 0x03 << 5;

		//PID
		*(pBuffer + 2) &= 0xE0;
		*(pBuffer + 2) |= (pr->PMTPID >> 8);
		*(pBuffer + 3) = pr->PMTPID >> 0;

		++it;

		pBuffer = pBuffer + 4;
	}

	return 0;
}