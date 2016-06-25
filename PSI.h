#pragma once

#include <vector>
using namespace std;

namespace com{
	namespace cloume{
		namespace cap{ 
			namespace streaming{

				class PSI{
				protected:
					PSI();

					virtual ~PSI(void){}

				public:
					char *Bitstream();
					virtual unsigned long BitstreamSize() = 0;

				protected:
					virtual void UpdateBitstream();

					//virtual unsigned char PointerField(){ return 0; }
					virtual unsigned char TableId() = 0;
					//virtual unsigned char SectionSyntaxIndicator(){ return 1; }
					virtual unsigned short SectionLength() = 0;
					virtual unsigned short UserDefinedData() = 0;	//PAT:transport stream ID; PMT:Program num;
					//virtual unsigned short ValidityIndicator() = 0;	//Current/next indicator
					virtual unsigned char SectionNumber() = 0;
					virtual unsigned char LastSectionNumber() = 0;

					virtual int BitstreamAfterTableInfo(unsigned char*) = 0;

				private:
					vector<char> mBitstream;
				};

			}
		}
	}
}