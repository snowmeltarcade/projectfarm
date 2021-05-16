#include <sstream>

#include "packet_factory.h"

#include "packets/server_client_load_world.h"
#include "packets/client_server_world_loaded.h"
#include "packets/server_client_entity_update.h"
#include "packets/server_client_set_player_details.h"
#include "packets/client_server_test_udp.h"
#include "packets/client_server_entity_update.h"
#include "packets/server_client_player_joined_world.h"
#include "packets/server_client_player_left_world.h"
#include "packets/server_client_remove_entity_from_world.h"
#include "packets/server_client_character_set_details.h"
#include "packets/client_server_player_authenticate.h"
#include "packets/client_server_request_hashed_password.h"
#include "packets/server_client_send_hashed_password.h"
#include "packets/client_server_chatbox_message.h"
#include "packets/server_client_chatbox_message.h"

namespace projectfarm::shared::networking
{
	std::shared_ptr<Packet> PacketFactory::CreatePacket(PacketTypes type)
	{
		std::shared_ptr<Packet> packet = nullptr;

		switch (type)
		{
		    case PacketTypes::ServerClientLoadWorld:
            {
                packet = std::make_shared<packets::ServerClientLoadWorldPacket>();
                break;
            }
		    case PacketTypes::ClientServerWorldLoaded:
            {
                packet = std::make_shared<packets::ClientServerWorldLoaded>();
                break;
            }
		    case PacketTypes::ServerClientEntityUpdate:
            {
                packet = std::make_shared<packets::ServerClientEntityUpdatePacket>();
                break;
            }
		    case PacketTypes::ServerClientSetPlayerDetails:
            {
                packet = std::make_shared<packets::ServerClientSetPlayerDetails>();
                break;
            }
		    case PacketTypes::ClientServerTestUdp:
            {
                packet = std::make_shared<packets::ClientServerTestUdp>();
                break;
            }
            case PacketTypes::ClientServerEntityUpdate:
            {
                packet = std::make_shared<packets::ClientServerEntityUpdatePacket>();
                break;
            }
		    case PacketTypes::ServerClientPlayerJoinedWorld:
            {
                packet = std::make_shared<packets::ServerClientPlayerJoinedWorld>();
                break;
            }
            case PacketTypes::ServerClientPlayerLeftWorld:
            {
                packet = std::make_shared<packets::ServerClientPlayerLeftWorld>();
                break;
            }
		    case PacketTypes::ServerClientRemoveEntityFromWorld:
            {
                packet = std::make_shared<packets::ServerClientRemoveEntityFromWorld>();
                break;
            }
            case PacketTypes::ServerClientCharacterSetDetails:
            {
                packet = std::make_shared<packets::ServerClientCharacterSetDetailsPacket>();
                break;
            }
            case PacketTypes::ClientServerPlayerAuthenticate:
            {
                packet = std::make_shared<packets::ClientServerPlayerAuthenticatePacket>();
                break;
            }
            case PacketTypes::ClientServerRequestHashedPassword:
            {
                packet = std::make_shared<packets::ClientServerRequestHashedPasswordPacket>();
                break;
            }
            case PacketTypes::ServerClientSendHashedPassword:
            {
                packet = std::make_shared<packets::ServerClientSendHashedPasswordPacket>();
                break;
            }
            case PacketTypes::ClientServerChatboxMessage:
            {
                packet = std::make_shared<packets::ClientServerChatboxMessagePacket>();
                break;
            }
            case PacketTypes::ServerClientChatboxMessage:
            {
                packet = std::make_shared<packets::ServerClientChatboxMessagePacket>();
                break;
            }
		}

		if (!packet)
		{
			std::ostringstream ss;
			ss << "Unknown packet type: '";
			ss << static_cast<int>(type);
			ss << "'";
			throw std::runtime_error(ss.str().c_str());
		}

		return packet;
	}
}