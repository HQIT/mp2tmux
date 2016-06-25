#include "Ticker.h"

using namespace com::cloume::common;

Ticker::Ticker(unsigned int init)
	: mInit(init)
{}

unsigned int Ticker::TickCount(){
	return GetCurrentTickCount() - mInit;
}