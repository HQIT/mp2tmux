#pragma once

class Program;

class PMT :
	public PSI
{
public:
	explicit PMT(Program* program) : mProgram(program)
	{
		;
	}

	virtual ~PMT(void){
		;
	}

	unsigned short PID();

protected:
	
	virtual unsigned char TableId(){
		return 0x02;
	}

	virtual unsigned short SectionLength();

	virtual unsigned short PMT::UserDefinedData();

	virtual unsigned char SectionNumber(){
		return 0x00;
	}

	virtual unsigned char LastSectionNumber(){
		return 0x00;
	}

	virtual unsigned long BitstreamSize();
	virtual int BitstreamAfterTableInfo(unsigned char*);

protected:
	virtual unsigned short PCRPID();
	virtual unsigned short ProgramInfoLength();

private:
	Program *mProgram;
};

