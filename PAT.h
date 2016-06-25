#pragma once

#include <vector>
#include "PSI.h"

#pragma warning (disable: 4267)

namespace com{
	namespace cloume{
		namespace cap{ namespace streaming{

			class PAT :
				public PSI
			{
			protected:
				struct ProgramRecord{
					unsigned int ProgramNumber	:16;
					unsigned int Reserved		:3;
					unsigned int PMTPID		:13;

					ProgramRecord(unsigned short ProgramNumber, unsigned short PMTPID){
						this->ProgramNumber = ProgramNumber;
						this->PMTPID = PMTPID;
						this->Reserved = 0x7;	//111
					}
				};

				typedef std::vector<ProgramRecord*> ProgramRecordVector;

			public:
				PAT(void){
					this->UpdateBitstream();
				}

				virtual ~PAT(void){
					ProgramRecordVector::iterator it = mProgramRecords.begin();
					while(it != mProgramRecords.end()){
						delete *it;
						++it;
					}

					mProgramRecords.clear();
				}

			public:

				virtual int AddProgram(unsigned short ProgramNumber, unsigned short PMTPID){
					mProgramRecords.push_back(new ProgramRecord(ProgramNumber, PMTPID));
					
					this->UpdateBitstream();
					
					return 0;
				}

				virtual unsigned long BitstreamSize();

			protected:
				virtual unsigned char TableId(){
					return 0x00;
				}

				virtual unsigned short SectionLength(){
					//Informs how many programs are listed below by specifying the number of bytes of this section, 
					//starting immediately following this field and including the CRC. First two bits must be zero.
					return 5 + 4 * mProgramRecords.size() + 4/*CRC32*/;
				}

				virtual unsigned short UserDefinedData(){
					return 0;
				}

				virtual unsigned char SectionNumber(){
					//TODO
					return 0;
				}

				virtual unsigned char LastSectionNumber(){
					//TODO:
					return 0;
				}

				virtual int BitstreamAfterTableInfo(unsigned char*);

			private:
				ProgramRecordVector mProgramRecords;
			};

		}
		}
	}
}