#pragma once

#include "MutableHeaderBuffer.h"

namespace com{
	namespace cloume{
		namespace cap{
			namespace streaming{
				
				class IDeliverer{
				public:
					virtual int Deliver(const char *, unsigned int, unsigned int timestamp = 0) = 0;
				};

			}
		}
	}
}