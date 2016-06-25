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
				// Description: ��ȡbuffer���Ѿ����ڵ��ֽ���
				//-
				virtual unsigned int Length() const;
				virtual void Length(unsigned int);

				//-
				// Method:    Full
				// FullName:  com::cloume::common::BasicBinaryBuffer::Full
				// Access:    public 
				// Returns:   bool
				// Qualifier:
				// Description: mBuffer�Ƿ����
				//-
				virtual bool Full();
				virtual bool Empty();

				virtual void Reset();

			protected:
				char *End();

			private:
				vector<char> mBuffer;
				unsigned int mLength;	//���һ��д���ֽڵ���һ���ֽڵľ���
			};

		}
	}
}