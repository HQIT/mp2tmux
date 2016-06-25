#pragma once

#include "BasicBinaryBuffer.h"
using namespace com::cloume::common;

namespace com{
	namespace cloume{
		namespace cap{ 
			namespace streaming{

				enum ESDataType{
					ESDT_INVALID = -1,
					ESDT_VIDEO,
					ESDT_AUDIO
				};

				class PES : public BasicBinaryBuffer{
					unsigned int mTimestamp;
				public:
					PES(char *, unsigned int, unsigned int timestamp);
					unsigned int Timestamp();
				};

				class PESHeader : public BasicBinaryBuffer{
				public:
					PESHeader(ESDataType type, unsigned int payloadLength, unsigned int timestamp = 0);

				public:
					unsigned int Length();

				private:
					const static int SIMPLE_PES_HEADER_LENGTH = 14;
				};
			}
		}
	}
}