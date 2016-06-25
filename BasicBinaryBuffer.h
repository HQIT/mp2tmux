#pragma once

#include <vector>
using namespace std;

namespace com{
	namespace cloume{
		namespace common{
			
			class BasicBinaryBuffer{
			public:
				explicit BasicBinaryBuffer(unsigned int capacity);
				virtual ~BasicBinaryBuffer();

			public:
				virtual int Write(const char *from, unsigned int fromSize, unsigned int off = 0);
				virtual char *Read(unsigned int off, unsigned int &count);
				virtual char *Read(unsigned int off);
				virtual char *Read();

				virtual unsigned int Capacity();
				//-
				// Method:    Size
				// FullName:  com::cloume::common::BasicBinaryBuffer::Size
				// Access:    public 
				// Returns:   unsigned int
				// Qualifier:
				// Description: 获取buffer中已经存在的字节数
				//-
				virtual unsigned int Length() const;
				virtual void Length(unsigned int);

				//-
				// Method:    Full
				// FullName:  com::cloume::common::BasicBinaryBuffer::Full
				// Access:    public 
				// Returns:   bool
				// Qualifier:
				// Description: mBuffer是否放满
				//-
				virtual bool Full();
				virtual bool Empty();

				virtual void Reset();

			protected:
				char *End();

			private:
				vector<char> mBuffer;
				unsigned int mLength;	//最后一个写入字节到第一个字节的距离
			};

		}
	}
}