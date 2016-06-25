#pragma once

#include <memory>
#include <stdint.h>

#pragma warning (disable: 4244)

namespace com{
	namespace cloume{
		namespace cap{ 
			namespace streaming{
				class Packet
				{
				public:
					const static unsigned int PACKET_LENGTH = 188;
					const static unsigned int PACKET_HEADER_LENGTH = 4;

					class PacketHeader{
					public:
						PacketHeader() : mData(NULL) {}

						void MapTo(unsigned char* pData){
							mData = pData;

							memset(mData, 0x00, sizeof(PacketHeader));
						}

						void SB(unsigned char sb){//:8 Sync Byte
							mData[0] = sb;
						}

						void TEI(unsigned char tei){	//	:1 Transport Error Indicator
							mData[1] &= 0x7F;
							mData[1] |= ((tei & 0x01) << 7);
						}
						unsigned char TEI(){
							return (mData[1] >> 7) & 0x01;
						}

						void PUSI(unsigned char pusi){	//	:1 Payload Unit Start Indicator
							mData[1] &= 0xBF;
							mData[1] |= ((pusi & 0x01) << 6);
						}
						unsigned char PUSI(){
							return (mData[1] >> 6) & 0x01;
						}

						void TP(unsigned char tp){		//	:1 Transport Priority
							mData[1] &= 0xDF;
							mData[1] |= ((tp & 0x01) << 5);
						}
						unsigned char TP(){
							return (mData[1] >> 5) & 0x01;
						}

						void PID(unsigned short pid){	//	:13
							mData[1] &= 0xE0;
							mData[1] |= (pid >> 8);
							mData[2] = (unsigned char)pid;
						}
						unsigned short PID(){
							return ((mData[1] & 0x1F) << 8) | mData[2];
						}

						void SC(unsigned char sc){		//	:2 Scrambling control
							mData[3] &= 0x3F;
							mData[3] |= ((sc & 0x03) << 6);
						}
						unsigned char SC(){
							return (mData[3] >> 6) & 0x03;
						}

						void AFE(unsigned char afe){	//	:2 Adaptation field exist
							mData[3] &= 0xCF;
							mData[3] |= ((afe & 0x03) << 4);
						}
						unsigned char AFE(){
							return (mData[3] >> 4) & 0x03;
						}

						void CC(unsigned char cc){		//	:4 Continuity counter
							mData[3] &= 0xF0;
							mData[3] |= (cc & 0x0F);
						}
						unsigned char CC(){
							return mData[3] & 0x0F; 
						}
					private:
						unsigned char* mData;
					};

					explicit Packet(unsigned short pid, bool pusi = false, unsigned char afe = 0x01)
						: mFilledSize(0)
					{
						mHeader.MapTo(mData);
						memset(mData + 4, 0xFF, sizeof(mData) - PACKET_HEADER_LENGTH);

						Header()->SB(0x47);
						Header()->PID(pid);
						Header()->PUSI(pusi ? 0x01 : 0x00);
						Header()->AFE(afe);
					}

					virtual ~Packet(void){}

				public:
					unsigned char* Data(){
						return mData;
					}

					unsigned char* Body(){
						return Data() + PACKET_HEADER_LENGTH;
					}

					unsigned char* Payload(){
						return Body() + AdaptationFieldLength();
					}

					unsigned long Fill(unsigned char* data, unsigned long);

					void Reset();

					void SetCC(unsigned char);
					unsigned char GetCC(){ return Header()->CC(); }

					void AFE(unsigned char);
					unsigned char AFE();

					unsigned short PID(){ return Header()->PID(); }

					void SetPCR(unsigned long msec){
						if(Header()->AFE() & 0x02){
							mData[4] = 1/*bit flags*/ + 6/*PCR*/;	//we ONLY have PCR in Adaptation Field
							mData[5] = 0x10;	//set PCR flag
						}else{
							return;
						}

						unsigned short PCR_ext = (27000 * msec) % 300;
						unsigned long PCR_base = msec * 90;	// (27000 * t / 300) % (1 << 33)

						*(mData + 6) = PCR_base >> 25;
						*(mData + 7) = PCR_base >> 17;
						*(mData + 8) = PCR_base >> 9;
						*(mData + 9) = PCR_base >> 1;

						*(mData + 10) = ((PCR_base << 7) & 0x80) | ((PCR_ext >> 8) & 0x01);//PCR_ext >> 8 | 0x7E | (PCR_base & 0x01);
						*(mData + 11) = PCR_ext >> 0;
					}

					virtual unsigned long PayloadCapacity(){

						unsigned long capacity = 0;

						if(Header()->AFE() == 0x01){		//01 - payload only
							capacity = PACKET_LENGTH - PACKET_HEADER_LENGTH;
						}else if(Header()->AFE() == 0x02){	//10 - adaptation field only
							return 0;
						}else if(Header()->AFE() == 0x03){	//11 - both
							capacity = PACKET_LENGTH - PACKET_HEADER_LENGTH - AdaptationFieldLength();
						}

						return capacity - mFilledSize;
					}

				protected:
					inline PacketHeader* Header(){
						return &mHeader;
					}

					inline unsigned char AdaptationFieldLength(){
						if(Header()->AFE() & 0x02){	//Adaptation Field exists
							return mData[4] + 1;	//First byte in Adaptation Field indicate it's length
						}else{
							return 0;
						}
					}

					inline void AdaptationFieldLength(unsigned char len){
						if(Header()->AFE() & 0x02){	//Adaptation Field exists
							mData[4] = len;
							mData[5] = 0;
						}
					}

				private:
					unsigned char mData[PACKET_LENGTH];
					unsigned long mFilledSize;
					PacketHeader mHeader;
				};

			}
		}
	}
}