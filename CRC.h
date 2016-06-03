#pragma once

namespace cap{

	class CRC32{
		static unsigned int CRC_32_Tab[256];

	public:
		static unsigned long CaculateCRC(unsigned char *data, unsigned long length);
	};
}