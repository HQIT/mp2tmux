#include "Packet.h"
#include "Program.h"

#include "PES.h"
#include "MP2TMuxer.h"
#include "PSI.h"
#include "PAT.h"

#include <algorithm>

using namespace com::cloume::cap;
using namespace com::cloume::cap::streaming;

MP2TMuxer::MP2TMuxer(bool period, double fps/* = 25.*/) 
	: mpDeliverer(NULL), mPSIPeriod(0), 
	mPeriodicSendPSI(period), mFPS(fps), mFrameCount(0),
	mpTicker(NULL), mLastPCR(0), mVideoStartTimestamp(0)
{
	mContinuityCounter[0x00] = 0x00;		//PAT
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

	/*PESQueueMap::iterator it3 = mPESQueueMap.begin();
	while(it3 != mPESQueueMap.end()){
		delete it3->second;
		++it3;
	}
	mPESQueueMap.clear();*/

	if(mpDeliverer){
		delete mpDeliverer;
		mpDeliverer = NULL;
	}

	if(mpTicker){
		delete mpTicker;
		mpTicker = NULL;
	}
}

Ticker *MP2TMuxer::GetTicker(){
	return mpTicker;
}

void MP2TMuxer::SetTicker(Ticker *ticker){
	mpTicker = ticker;
}

void MP2TMuxer::SetDeliverer(IDeliverer *pDeliverer){
	this->mpDeliverer = pDeliverer;
}

IDeliverer *MP2TMuxer::GetDeliverer(){
	return mpDeliverer;
}

void MP2TMuxer::AddProgram(Program* program){
	mContinuityCounter[program->PMTPID()] = 0x00;
	mPrograms.push_back(program);

	mPAT.AddProgram(program->Number(), program->PMTPID());
	mPMTs.push_back(new PMT(program));

	mPCRPIDs.push_back(program->PCRPID());

	//Intialize the CC for each Stream, and prepare the PES queue
	Program::StreamVector &sv = program->Streams();
	Program::StreamVector::iterator cit = sv.begin();
	Program::Stream *s = NULL;
	for(;cit != sv.end();){
		s = *cit;
		mContinuityCounter[s->ElementaryPID()] = 0x00;
		///mPESQueueMap[s->ElementaryPID()] = new PESQueue();
		///s->LastFrameTimestamp(GetTicker()->TickCount());
		++cit;
	}
}

Program *MP2TMuxer::GetProgram(int idx /* = 0 */){
	if(mPrograms.size() < idx + 1){
		return NULL;
	}

	return mPrograms[idx];
}

//buf中为ES
int MP2TMuxer::Mux(Program::Stream *s, MutableHeaderBuffer *buf, unsigned int duration){
	unsigned int timestamp = GetTicker()->TickCount();

	static bool isFirst = true;
	static unsigned int lasttsv = 0, lasttsa = 0;
	static int cnt = 0;

	ESDataType dt = ESDT_INVALID;
	if(s->Type() == Program::Stream::StreamType_Video){
		dt = ESDT_VIDEO;
		lasttsv = (timestamp = (GetTicker()->TickCount() - getVideoStartTimestamp()));	//ms
		//printf("video timestamp: %d \n", timestamp);
	}else if(s->Type() == Program::Stream::StreamType_Audio){
		dt = ESDT_AUDIO;
		lasttsa = timestamp = cnt * 24;
		//if(timestamp > lasttsv){ //丢弃部分声音
		//	return 0;
		//}
		//printf("audio timestamp: %d \n", timestamp);
		cnt += 1;
	}

	//PESHeader
	PESHeader header(dt, buf->HeadlessDataLength(), timestamp);
	buf->AppendHeader(header.Read(), header.Length());
	
	PES *pes = new PES(buf->HeadedData(), buf->HeadedDataLength(), timestamp);

	if(dt == ESDT_VIDEO){
		PESData d;
		d.pid = s->ElementaryPID();
		d.tsx1000 = timestamp * 1000;
		d.dpp = 33 * 1000 * 184. / pes->Length();
		d.pes = pes;
		mVideoPESQueue.push(d);
	}else if(dt == ESDT_AUDIO){
		PESData d;
		d.pid = s->ElementaryPID();
		d.tsx1000 = timestamp * 1000;
		d.dpp = 24 * 1000 * 184. / pes->Length();
		d.pes = pes;
		mAudioPESQueue.push(d);
	}

	return 0;
}

void MP2TMuxer::Deliver(){
	if(mPSIPeriod <= 0 && mPeriodicSendPSI){
		mPSIPeriod = PATPMT_INTERVAL;
		DeliverPATPMT();
	}

	static unsigned int lastVideoTS = 0;
	static unsigned int lastAudioTS = 0;
	static unsigned int pkcnt = 0;

	do{
		PESData *pda = NULL, *pdv = NULL;

		///VIDEO
		if(mVideoPESQueue.size() > 0){
			pdv = &mVideoPESQueue.front();
		}

		///AUDIO
		if(mAudioPESQueue.size() > 0){
			pda = &mAudioPESQueue.front();
		}

		if(pdv == NULL){
			break;
		}

		bool needpop = false;
		if(!pda || pda->tsx1000 > pdv->tsx1000){
			Packet packet(pdv->pid, pdv->offset == 0);

			if(pdv->pes->Length() - pdv->offset < packet.PayloadCapacity()){
				packet.AFE(0x03);
				needpop = true;
			}
			if(pdv->offset == 0){
				packet.AFE(0x03);
				packet.SetPCR(mLastPCR = (GetTicker()->TickCount() - getVideoStartTimestamp()));
			}

			///sned a v packet
			unsigned long ate = packet.Fill((unsigned char *)pdv->pes->Read(pdv->offset), pdv->pes->Length() - pdv->offset);
			this->DeliverPacket(packet);
			pdv->offset += ate;
			pdv->tsx1000 += pdv->dpp;

			if(needpop){
				delete pdv->pes;
				mVideoPESQueue.pop();
			}
		}else{
			///sned a aud packet
			Packet packet(pda->pid, pda->offset == 0);
			if(pda->pes->Length() - pda->offset < packet.PayloadCapacity()){
				packet.AFE(0x03);
				needpop = true;
			}

			unsigned long ate = packet.Fill((unsigned char *)pda->pes->Read(pda->offset), pda->pes->Length() - pda->offset);
			this->DeliverPacket(packet);
			pda->offset += ate;
			pda->tsx1000 += pda->dpp;

			if(needpop){
				delete pda->pes;
				mAudioPESQueue.pop();
			}
		}

	}while(true);
}

int MP2TMuxer::PackAndDeliver(unsigned short pid, unsigned char* data, unsigned long size, unsigned int timestamp, bool pes/* = true*/){

	Packet packet(pid, true);

	//Fill into TS
	unsigned long idx = 0;
	while(idx < size){
		if(size - idx < packet.PayloadCapacity() && pes){	//一帧发不完的PES, 末尾需要填充!
			packet.AFE(0x03);
		}
		unsigned long ate = packet.Fill(data + idx, size - idx);
		this->DeliverPacket(packet);
		idx += ate;
	}

	return 0;
}

//@data: TS packet
int MP2TMuxer::DeliverPacket2(Packet &packet, std::vector<PacketData> &packs){
	return 0;
}

//@data: TS packet
int MP2TMuxer::DeliverPacket(Packet &packet){
	unsigned short pid = packet.PID();
	
	if(this->mpDeliverer){
		mPSIPeriod -= 1;

		packet.SetCC(mContinuityCounter[pid]);
		int ret = this->mpDeliverer->Deliver((const char *)packet.Data(), Packet::PACKET_LENGTH);
		packet.Reset();
		mContinuityCounter[pid] = packet.GetCC();

		return ret;
	}
	if (mDeliverer) {
		mPSIPeriod -= 1;

		packet.SetCC(mContinuityCounter[pid]);
		int ret = this->mDeliverer((const unsigned char *)packet.Data(), Packet::PACKET_LENGTH, mDeliverHandler);
		packet.Reset();
		mContinuityCounter[pid] = packet.GetCC();
	}
	
	return 0;
}

void MP2TMuxer::DeliverPCR(){
	Packet packet(256, false, 0x03);
	packet.SetCC(mContinuityCounter[256]);
	///packet.SetPCR(GetTicker()->TickCount());
	packet.SetPCR(mLastPCR / 1000.);

	this->DeliverPacket(packet);

	//packet.Reset();
	mContinuityCounter[256] = packet.GetCC();
}

void MP2TMuxer::DeliverPATPMT(){
	//PAT
	unsigned char * pBuffer = (unsigned char *)mPAT.Bitstream();
	unsigned int size = mPAT.BitstreamSize();

	PackAndDeliver(0x00, pBuffer, size, 0, false);

	//PMTs
	PMTVector::iterator it = mPMTs.begin();
	while(it != mPMTs.end()){
		PMT* pmt = *it;
		pBuffer = (unsigned char *)pmt->Bitstream();
		size = pmt->BitstreamSize();
		PackAndDeliver(pmt->PID(), pBuffer, size, 0, false);
		
		++it;
	}
}