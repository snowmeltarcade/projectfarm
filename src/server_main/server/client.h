#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <cstdint>

#include <SDL_net.h>

namespace projectfarm::server
{
	class Client final
	{
	public:
		Client(IPaddress* ipAddress, TCPsocket socket)
			: _ipAddress(ipAddress),
			  _socket(socket)
		{}
		~Client()
        {
            this->Disconnect();
        }

		[[nodiscard]]
        bool IsConnected() const noexcept;

		void Disconnect() noexcept;

		[[nodiscard]]
        std::string IPAddressAsString() const noexcept;

		[[nodiscard]]
        TCPsocket GetSocket() const noexcept
		{
			return this->_socket;
		}

	private:
		IPaddress* _ipAddress;
		TCPsocket _socket;
	};
}

#endif