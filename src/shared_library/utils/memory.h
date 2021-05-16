#ifndef MEMORY_H
#define MEMORY_H

namespace projectfarm::shared::utils
{
	enum class Endian
	{
		Big = 0,
		Little = 2,
	};

	Endian GetEndian();
}

#endif