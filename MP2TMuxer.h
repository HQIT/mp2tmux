#pragma once

#include <map>
#include <vector>
#include <stdint.h>

#include "PAT.h"
#include "PMT.h"
#include "Program.h"
#include "Packet.h"

#include <Windows.h>

class MP2TMuxer
{
public:

	const static int PATPMT_INTERVAL = 255;

	MP2TMuxer(bool mPeriodicSendPSI = false, double fps = 30.);
	virtual ~MP2TMuxer(void);

	typedef std::pair<unsigned short, unsigned char> ContinuityCounterPair;
	typedef std::map<unsigned short, unsigned char> ContinuityCounterMap;

public:
	void AddProgram(Program*);
	//int MuxVideo(Program::StreamVideo*, unsigned char*, unsigned long);
	//int MuxAudio(Program::StreamAudio*, unsigned char*, unsigned long);
	int Mux(Program::Stream*, unsigned char*, unsigned long, bool sleep = false);
	
	typedef int (*PacketsDeliverer)(const unsigned char*, const unsigned long, void*);
	void SetPacketsDeliverer(PacketsDeliverer deliverer, void* handler){
		mDeliverer = deliverer;
		mDeliverHandler = handler;
	}

	void DeliverPATPMT();
	//void Reset();
	unsigned long &FrameCount(){ return mFrameCount; }

protected:

	typedef std::vector<PMT*> PMTVector;
	typedef std::vector<unsigned short> PCRPIDVector;

private:
	inline int PreMux(Program::Stream*, unsigned char *header, unsigned long size);

	inline int DeliverPacket(const unsigned char* data, unsigned long size = Packet::PACKET_LENGTH);
	int Deliver(unsigned short pid, unsigned char* data, unsigned long size);
	
private:
	ProgramVector mPrograms;
	PacketsDeliverer mDeliverer;
	void* mDeliverHandler;
	ContinuityCounterMap mContinuityCounter;

	//send PAT & PMT every N packets
	long mPSIPeriod;
	bool mPeriodicSendPSI;

	PAT mPAT;
	PMTVector mPMTs;
	PCRPIDVector mPCRPIDs;
	double mFPS;
	double mFrameDuration;
	unsigned long mFrameCount;		//Simple PCR

	//SYSTEMTIME mBaseST;
};

