#include "Program.h"

Program::Program(unsigned short number, unsigned short pmtpid) : mNumber(number), mPMTPID(pmtpid){
	mStreamsCounter[Stream::StreamType_Video] = 0;
	mStreamsCounter[Stream::StreamType_Audio] = 0;
	mStreamsCounter[Stream::StreamType_Subtitle] = 0;
}

Program::~Program(void){
	StreamVector::iterator it = mStreams.begin();
	while(it != mStreams.end()){
		delete *it;
		++it;
	}

	mStreams.clear();
}

void Program::AddStream(Stream* stream){

	stream->SetIndex(mStreamsCounter[stream->Type()]);
	mStreamsCounter[stream->Type()] += 1;

	mStreams.push_back(stream);
}