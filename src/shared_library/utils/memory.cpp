#include "memory.h"

#include <cstdint>

namespace projectfarm::shared::utils
{
	Endian GetEndian()
	{
		const uint16_t value = 0x1234;

		auto msb = static_cast<const unsigned char*>(static_cast<const void*>(&value));

		return msb[0] == 0x12 ? Endian::Big : Endian::Little;
	}
}