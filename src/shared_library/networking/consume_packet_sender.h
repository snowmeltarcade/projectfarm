#ifndef CONSUME_PACKET_SENDER_H
#define CONSUME_PACKET_SENDER_H

#include <memory>

#include "packet_sender.h"

namespace projectfarm::shared::networking
{
	class ConsumePacketSender
	{
	public:
		ConsumePacketSender() = default;
		virtual ~ConsumePacketSender() = default;

		void SetPacketSender(const std::shared_ptr<PacketSender>& packetSender)
		{
			this->_packetSender = packetSender;
		}

	protected:
		std::shared_ptr<PacketSender> _packetSender;
	};
}

#endif
