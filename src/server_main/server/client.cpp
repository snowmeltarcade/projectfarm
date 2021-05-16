#include "client.h"

#include <sstream>

namespace projectfarm::server
{
	bool Client::IsConnected() const noexcept
	{
		return this->_socket != nullptr;
	}

	void Client::Disconnect() noexcept
	{
		if (!this->IsConnected())
        {
            return;
        }

		SDLNet_TCP_Close(this->_socket);

		this->_socket = nullptr;
		this->_ipAddress = nullptr;
	}

	std::string Client::IPAddressAsString() const noexcept
	{
	    if (this->_ipAddress == nullptr)
        {
	        return "No IP Address - Disconnected";
        }

		auto host = static_cast<unsigned int>(this->_ipAddress->host);

		// for some reason, SDLNet seems to store the IP address backwards...
		unsigned int part4 = (host & 0xFF000000) >> 24u;
		unsigned int part3 = (host & 0x00FF0000) >> 16u;
		unsigned int part2 = (host & 0x0000FF00) >> 8u;
		unsigned int part1 = (host & 0x000000FF) >> 0u;

		std::ostringstream os;

		os << part1 << "."
			<< part2 << "." 
			<< part3 << "." 
			<< part4 << ":"
			<< this->_ipAddress->port;

		return os.str();
	}
}