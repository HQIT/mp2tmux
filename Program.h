#pragma once

#include <vector>
#include <map>

namespace com{
	namespace cloume{
		namespace cap{ namespace streaming{
class Program
{
public:
	class Stream{
	public:
		enum EnumStreamType{
			StreamType_Video = 0,
			StreamType_Audio,
			StreamType_Subtitle,
			StreamType_PCR,
		};

		enum EnumStreamSubtype{
			STREAMSUBTYPE_H264_VIDEO = 0x1b,
			STREAMSUBTYPE_ISO_IEC_13818_3_AUDIO = 0x04,	//MPEG-II
			STREAMSUBTYPE_ISO_IEC_11172_3_AUDIO = 0x03,	//MPEG-I
			STREAMSUBTYPE_PCR = 0xFF,
		};

	public:
		EnumStreamType Type(){ return mType; }
		EnumStreamSubtype SubType(){ return mSubtype; }
		unsigned short ElementaryPID(){ return mElementaryPID; }

		//index in Program's stream list
		unsigned char Index(){ return mIndex; }
		void SetIndex(unsigned char index){ mIndex = index; }

		unsigned int LastFrameTimestamp(){ return mLastFrameTimestamp; }
		void LastFrameTimestamp(unsigned int ts){ mLastFrameTimestamp = ts; }

	protected:
		Stream(EnumStreamType type, EnumStreamSubtype Subtype, unsigned short ElementaryPID)
			: mLastFrameTimestamp(0)
		{
			this->mType = type;
			this->mSubtype = Subtype;
			this->mElementaryPID = ElementaryPID;
		}

	private:
		EnumStreamType mType;
		EnumStreamSubtype mSubtype;
		unsigned short mElementaryPID;
		unsigned char mIndex;

		unsigned int mLastFrameTimestamp;
	};

	class StreamVideo : public Stream{
	public:
		StreamVideo(EnumStreamSubtype Subtype, unsigned short ElementaryPID) 
			: Stream(StreamType_Video, Subtype, ElementaryPID){
		}
	};

	class StreamAudio : public Stream{
	public:
		StreamAudio(EnumStreamSubtype Subtype, unsigned short ElementaryPID)
			: Stream(StreamType_Audio, Subtype, ElementaryPID){
		}
	};

	class StreamPCR : public Stream{
	public:
		StreamPCR(unsigned short ElementaryPID)
			: Stream(StreamType_Audio, Stream::STREAMSUBTYPE_PCR, ElementaryPID){
		}
	};

	typedef std::vector<Stream*> StreamVector;
	typedef std::pair<Stream::EnumStreamType, unsigned char> StreamCounterPair;
	typedef std::map<Stream::EnumStreamType, unsigned char> StreamCounterMap;

public:
	//@number: program number in PAT, @pid PMT's PID in PAT
	//pid 0x0010 in PAT is for NIT, program number 0x0000 is for NIT
	Program(unsigned short number = 1, unsigned short pid = 0x20);
	virtual ~Program(void);

public:
	//void AddStream(Stream::EnumStreamType type, unsigned char SubType, unsigned short ElementaryPID);
	void AddStream(Stream*);
	unsigned short Number(){ return mNumber; }
	unsigned short PMTPID(){ return mPMTPID; }
	unsigned short PCRPID(){ return mPCRPID; }
	void PCRPID(unsigned short pid){ mPCRPID = pid; }

	StreamVector &Streams(){ return mStreams; }

private:
	StreamVector mStreams;
	StreamCounterMap mStreamsCounter;	//stream count for each stream type
	unsigned short mNumber, mPMTPID, mPCRPID;
};

typedef std::vector<Program*> ProgramVector;

		}
		}
	}
}