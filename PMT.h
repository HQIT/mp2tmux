#pragma once

namespace com{
	namespace cloume{
		namespace cap{ namespace streaming{
			class Program;

			class PMT :
				public PSI
			{
			public:
				explicit PMT(Program* program) : mProgram(program)
				{
					this->UpdateBitstream();
				}

				virtual ~PMT(void){
					;//mProgram 由外部(muxer)清理
				}

				unsigned short PID();

				virtual unsigned long BitstreamSize();

			protected:

				virtual unsigned char TableId(){
					return 0x02;
				}

				virtual unsigned short SectionLength();

				virtual unsigned short UserDefinedData();

				virtual unsigned char SectionNumber(){
					return 0x00;
				}

				virtual unsigned char LastSectionNumber(){
					return 0x00;
				}

				virtual int BitstreamAfterTableInfo(unsigned char*);

			protected:
				virtual unsigned short PCRPID();
				virtual unsigned short ProgramInfoLength();

			private:
				Program *mProgram;
			};

		}
		}
	}
}