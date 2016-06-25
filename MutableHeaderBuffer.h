#pragma once

#include <vector>
using namespace std;

#include "BasicBinaryBuffer.h"
using namespace com::cloume::common;

namespace com{
	namespace cloume{
		namespace cap{

			class MutableHeaderBuffer : public BasicBinaryBuffer{
			public:
				explicit MutableHeaderBuffer(unsigned int dataCapacity, unsigned int headerCapacity = 128);
				virtual ~MutableHeaderBuffer();

			public:
				char *HeadlessData();
				char *HeadedData();

				bool DataFull();

				unsigned int HeadedDataLength();
				unsigned int HeadlessDataLength();

				unsigned int AppendHeader(const char *, unsigned int);

				unsigned int AppendData(const char *, unsigned int);

				unsigned int HeaderLength();
				unsigned int HeaderCapacity();

				void Reset();

			private:
				char *mpHeaderBegin;
				char *mpDataBegin;

				unsigned int mDataLength;
			};
		}
	}
}