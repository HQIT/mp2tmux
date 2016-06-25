#pragma once

#include <map>
#include <queue>
#include <vector>
#include <stdint.h>

#include "PAT.h"
#include "PMT.h"
#include "Program.h"
#include "Packet.h"
#include "PES.h"

#include "IDeliverer.h"
#include "MutableHeaderBuffer.h"
#include "Ticker.h"

using namespace com::cloume::common;

namespace com{ 
	namespace cloume{ 
		namespace cap{ 
			namespace streaming{
				class MP2TMuxer
				{
				public:

					const static int PATPMT_INTERVAL = 255;

					explicit MP2TMuxer(bool mPeriodicSendPSI = false, double fps = 25.);
					virtual ~MP2TMuxer(void);

					typedef std::pair<unsigned short, unsigned char> ContinuityCounterPair;
					typedef std::map<unsigned short, unsigned char> ContinuityCounterMap;

				public:
					void AddProgram(Program*);
					Program *GetProgram(int idx = 0);	//现在只支持一个Program
					int Mux(Program::Stream *, MutableHeaderBuffer *, unsigned int duration);
					//int Mux(Program::Stream *, MutableHeaderBuffer *, double duration);

					void SetDeliverer(IDeliverer *);
					
					typedef int(*PacketsDeliverer)(const unsigned char*, const unsigned long, void*);
					void SetPacketsDeliverer(PacketsDeliverer deliverer, void* handler) {
						mDeliverer = deliverer;
						mDeliverHandler = handler;
					}

					//周期性用该方法发送数据
					void Deliver();

					//void Reset();
					unsigned long &FrameCount(){ return mFrameCount; }
					//double FPS(){ return mFPS; }
					//double FrameDuration(){ return mFrameDuration; }

					void SetTicker(Ticker *);

				protected:

					double mLastPCR;
					///double mCurrentPCR;

					void DeliverPATPMT();
					void DeliverPCR();

					Ticker *GetTicker();

					unsigned int getVideoStartTimestamp() {
						if (mVideoStartTimestamp == 0) {
							mVideoStartTimestamp = GetTicker()->TickCount();
						}
						return mVideoStartTimestamp;
					}

					typedef std::vector<PMT*> PMTVector;
					typedef std::vector<unsigned short> PCRPIDVector;

				private:
					
					class PacketData{
					public:
						unsigned char *data;
						unsigned int ts;

						PacketData(unsigned char *indata, unsigned int tsx1000) : ts(tsx1000){
							data = new unsigned char[188];
							memcpy(data, indata, 188);
						}

						~PacketData(){
							delete[] data;
						}
					};

					//inline int DeliverPacket(const unsigned char* data, unsigned long size = Packet::PACKET_LENGTH);
					int DeliverPacket(Packet &packet);
					int DeliverPacket2(Packet &packet, std::vector<PacketData> &packs);
					
					//-
					// Method:    Deliver
					// FullName:  com::cloume::cap::streaming::MP2TMuxer::Deliver
					// Access:    private 
					// Returns:   int
					// Qualifier:
					// Parameter: unsigned short pid 数据所属的PID
					// Parameter: unsigned char * data PES帧数据或者PAT/PMT数据
					// Parameter: unsigned long size 帧数据字节数
					// Description: 分发整帧PES数据,或者PAT/PMT等数据!
					//-
					int PackAndDeliver(unsigned short pid, unsigned char* data, unsigned long size, unsigned int timestamp, bool pes = true);

					IDeliverer *GetDeliverer();

				public:
					double &FPS(){ return mFPS; }

				private:
					ProgramVector mPrograms;
					PacketsDeliverer mDeliverer;
					void* mDeliverHandler;

					IDeliverer *mpDeliverer;
					ContinuityCounterMap mContinuityCounter;

					//send PAT & PMT every N packets
					long mPSIPeriod;
					bool mPeriodicSendPSI;

					PAT mPAT;
					PMTVector mPMTs;
					PCRPIDVector mPCRPIDs;

					double mFPS;
					//double mFrameDuration;		//ms
					unsigned long mFrameCount;	//用作Simple PCR

					unsigned int mVideoStartTimestamp;

					class PESData{
					public:
						double tsx1000;
						double dpp;	//duration per packet
						unsigned int offset;

						PES *pes;
						unsigned short pid;

						PESData():tsx1000(0), dpp(0), offset(0), pes(NULL), pid(0){};
					};

					///typedef std::queue<PES *> PESQueue;
					typedef std::queue<PESData> PESDataQueue;
					PESDataQueue mAudioPESQueue;
					PESDataQueue mVideoPESQueue;

					///typedef std::map<unsigned short, PESQueue *> PESQueueMap;
					///PESQueueMap mPESQueueMap;

					Ticker *mpTicker;
				};

			}
		}
	}
}