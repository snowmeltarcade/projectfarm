#ifndef PACKET_TYPES_H
#define PACKET_TYPES_H

namespace projectfarm::shared::networking
{
	enum class PacketTypes
	{
		ServerClientLoadWorld = 0,
        ClientServerWorldLoaded = 1,
        ServerClientEntityUpdate = 2,
        ServerClientSetPlayerDetails = 3,
        ClientServerTestUdp = 4,
        ClientServerEntityUpdate = 5,
        ServerClientPlayerJoinedWorld = 6,
        ServerClientPlayerLeftWorld = 7,
        ServerClientRemoveEntityFromWorld = 8,
        ServerClientCharacterSetDetails = 9,
        ClientServerPlayerAuthenticate = 10,
        ClientServerRequestHashedPassword = 11,
        ServerClientSendHashedPassword = 12,
        ClientServerChatboxMessage = 13,
        ServerClientChatboxMessage = 14,
	};
}

#endif