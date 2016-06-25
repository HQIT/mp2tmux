#pragma once

namespace com{ 
	namespace cloume{ 
		namespace common{
			class Ticker{
			public:
				Ticker(unsigned int init);

				unsigned int TickCount();

			protected:
				virtual unsigned int GetCurrentTickCount() = 0;

			private:
				unsigned int mInit;
			};
		}
	}
}