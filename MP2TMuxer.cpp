#include "Packet.h"
#include "Program.h"

#include "PES.h"
#include "MP2TMuxer.h"
#include "PSI.h"
#include "PAT.h"

#include <algorithm>

#include <Windows.h>

MP2TMuxer::MP2TMuxer(bool period, double fps) 
	: mDeliverer(NULL), mPSIPeriod(0), mFPS(fps), mFrameDuration(1.0 / fps), 
	mDeliverHandler(NULL), mFrameCount(0), mPeriodicSendPSI(period){
		mContinuityCounter[0x00] = 0x00;		//PAT

		//GetLocalTime(&mBaseST);
}

MP2TMuxer::~MP2TMuxer(void)
{
	ProgramVector::iterator it = mPrograms.begin();
	while(it != mPrograms.end()){
		delete *it;
		++it;
	}
	mPrograms.clear();

	PMTVector::iterator it2 = mPMTs.begin();
	while(it2 != mPMTs.end()){
		delete *it2;
		++it2;
	}
	mPMTs.clear();
}

void MP2TMuxer::AddProgram(Program* program){
	mContinuityCounter[program->PMTPID()] = 0x00;
	mPrograms.push_back(program);

	mPAT.AddProgram(program->Number(), program->PMTPID());
	mPMTs.push_back(new PMT(program));

	mPCRPIDs.push_back(program->PCRPID());
}

int MP2TMuxer::Mux(Program::Stream* s, unsigned char* data, unsigned long size, bool sleep){

	DWORD i = GetTickCount();

	if(mContinuityCounter.count(s->ElementaryPID()) == 0){
		mContinuityCounter[s->ElementaryPID()] = 0x00;
	}

	//unsigned char* pesBuffer = new unsigned char[PES::SIMPLE_HEADER_LENGTH + size];
	
	//pack into PES, make PES header
	//PES pes(size);
	//pes.MakeHeader(pesBuffer, s);

	//memcpy(pesBuffer + PES::SIMPLE_HEADER_LENGTH, data, size);
	//FILE* f = fopen("c:/dump/foo.h264", "a+b");
	//FILE* f = fopen("c:/dump/foo.pes", "a+b");
	//fwrite(data, 1, size, f);
	//fwrite(pesBuffer + PES::SIMPLE_HEADER_LENGTH, size, 1, f);
	//fwrite(pesBuffer, PES::SIMPLE_HEADER_LENGTH + size, 1, f);
	//fclose(f);
	//printf("++ PES size: %d (0x%04x)\n", PES::SIMPLE_HEADER_LENGTH + size, PES::SIMPLE_HEADER_LENGTH + size);

	//Deliver(s->ElementaryPID(), pesBuffer, PES::SIMPLE_HEADER_LENGTH + size);
	Deliver(s->ElementaryPID(), data, size);

	//delete[] pesBuffer;

#if 0
	DWORD i2 = GetTickCount();

	if(sleep){
		if(i2 - i < 34){
//			Sleep(34 - i2 + i);
		}
	}
#endif

	return 0;
}

//@data: TS packet
inline int MP2TMuxer::DeliverPacket(const unsigned char* data, unsigned long size){

	if(mPSIPeriod <= 0 && mPeriodicSendPSI){
		mPSIPeriod = PATPMT_INTERVAL;
		DeliverPATPMT();
	}

	if(this->mDeliverer){

		mPSIPeriod -= (size / Packet::PACKET_LENGTH);

		return mDeliverer(data, size, mDeliverHandler);
	}

	return 0;
}

#if 0
double MP2TMuxer::DifferenceFromBaseST(){
	SYSTEMTIME stNow;
	FILETIME ft;

	GetLocalTime(&stNow);

	ULARGE_INTEGER ulBase, ulNow;

	SystemTimeToFileTime(&mBaseST, &ft);
	ulBase.HighPart = ft.dwHighDateTime;
	ulBase.LowPart = ft.dwLowDateTime;

	SystemTimeToFileTime(&stNow, &ft);
	ulNow.HighPart = ft.dwHighDateTime;
	ulNow.LowPart = ft.dwLowDateTime;

	uint64_t nDiff = ulNow.QuadPart - ulBase.QuadPart;
	//return (nDiff >> 14) / 52734375.f;
	return nDiff / 10000000.f;
	//return nDiff / 24. / 60. / 60. / 1000.;
}
#endif

int MP2TMuxer::Deliver(unsigned short pid, unsigned char* data, unsigned long size){

	int c = count(mPCRPIDs.begin(), mPCRPIDs.end(), pid);

	Packet packet(pid, true, c == 0 ? 0x01 : 0x03);
	packet.SetCC(mContinuityCounter[pid]);

	//SYSTEMTIME stUTC;
	//GetLocalTime(&stUTC);
	//packet.SetPCR(DifferenceFromBaseST());
	packet.SetPCR(mFrameCount/*, 25.*/);

	//then into TS
	unsigned long idx = 0;
	while(idx < size){
		unsigned long ate = packet.Fill(data + idx, size - idx);
		this->DeliverPacket(packet.Data());
		idx += ate;

		packet.Reset();	//update CC in
	}

	mContinuityCounter[pid] = packet.GetCC();

	return 0;
}

void MP2TMuxer::DeliverPATPMT(){
	//PAT
	unsigned char * pBuffer = NULL;
	unsigned long size = mPAT.Bitstream(pBuffer);

	Deliver(0x00, pBuffer, size);

	delete[] pBuffer;

	//PMTs
	PMTVector::iterator it = mPMTs.begin();
	while(it != mPMTs.end()){
		PMT* pmt = *it;

		size = pmt->Bitstream(pBuffer);
		Deliver(pmt->PID(), pBuffer, size);

		delete[] pBuffer;

		++it;
	}
}