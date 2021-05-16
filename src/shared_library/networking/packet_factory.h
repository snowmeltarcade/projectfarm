#ifndef PACKET_FACTORY_H
#define PACKET_FACTORY_H

#include <memory>

#include "packet.h"

namespace projectfarm::shared::networking
{
	class PacketFactory final
	{
	public:
        PacketFactory() = delete;
        ~PacketFactory() = delete;

		[[nodiscard]] static std::shared_ptr<Packet> CreatePacket(PacketTypes type);
	};
}

#endif
